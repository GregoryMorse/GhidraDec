#!/usr/bin/env python3
"""IDA SDK manifest helper for local builds and GitHub Actions."""

from __future__ import annotations

import argparse
import json
import os
import platform
import shutil
import subprocess
import sys
import time
import urllib.error
import urllib.parse
import urllib.request
import zipfile
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
MANIFEST = ROOT / "ghidradec.targets.json"
DEFAULT_SDK_DIR = ROOT / ".idasdks"
PUBLIC_SDK_SOURCES = {"public-release", "public-branch"}


def env_int(name: str, default: int) -> int:
    value = os.environ.get(name, "")
    if not value:
        return default
    try:
        return int(value)
    except ValueError:
        return default


def default_sdk_dir() -> Path:
    return Path(os.environ.get("GHIDRADEC_IDA_SDK_CACHE", str(DEFAULT_SDK_DIR))).resolve()


def load_manifest() -> dict:
    return json.loads(MANIFEST.read_text(encoding="utf-8"))


def host_platform() -> str:
    system = platform.system().lower()
    if system == "darwin":
        return "macos"
    if system.startswith("windows"):
        return "windows"
    if system == "linux":
        return "linux"
    return system


def find_sdk(manifest: dict, version: str) -> dict:
    for sdk in manifest["idaSdks"]:
        if sdk["version"] == version:
            return sdk
    raise SystemExit(f"IDA SDK {version} is not defined in {MANIFEST}")


def cmake_command() -> str:
    if found := shutil.which("cmake"):
        return found

    candidate_roots = [
        Path(os.environ[key])
        for key in ("ProgramFiles", "ProgramFiles(x86)")
        if os.environ.get(key)
    ]
    for root in candidate_roots:
        candidate = root / "CMake" / "bin" / "cmake.exe"
        if candidate.exists():
            return str(candidate)

        visual_studio_root = root / "Microsoft Visual Studio"
        if visual_studio_root.exists():
            for candidate in visual_studio_root.glob("*/**/Common7/IDE/CommonExtensions/Microsoft/CMake/CMake/bin/cmake.exe"):
                if candidate.exists():
                    return str(candidate)

    return "cmake"


def run(command: list[str], dry_run: bool = False) -> subprocess.CompletedProcess[str] | None:
    print("+ " + " ".join(command))
    if dry_run:
        return None
    return subprocess.run(command, cwd=ROOT, check=True, text=True)


def capture(command: list[str], dry_run: bool = False) -> str:
    print("+ " + " ".join(command))
    if dry_run:
        return ""
    result = subprocess.run(
        command,
        cwd=ROOT,
        check=True,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
    )
    return result.stdout


def parse_versions(value: str | None) -> set[str]:
    return {version.strip() for version in (value or "").split(",") if version.strip()}


def sdk_version_major(version: str) -> str:
    return version.split(".", 1)[0]


def expand_version_selectors(selectors: set[str], known_versions: set[str]) -> set[str]:
    expanded = set()
    unknown = set()
    for selector in selectors:
        if selector in known_versions:
            expanded.add(selector)
        elif selector in {"6", "7", "8", "9"}:
            expanded.update(version for version in known_versions if sdk_version_major(version) == selector)
        else:
            unknown.add(selector)
    if unknown:
        raise SystemExit(
            "Unknown IDA SDK version selector(s): "
            + ", ".join(sorted(unknown))
            + ". Use exact versions, latest, major groups 6/7/8/9, or all."
        )
    return expanded


def parse_arches(value: str | None) -> set[str]:
    arches = set()
    for arch in (value or "").split(","):
        arch = arch.strip().lower()
        if not arch or arch == "all":
            continue
        if arch in {"x86_64", "amd64"}:
            arch = "x64"
        elif arch in {"arm", "aarch64"}:
            arch = "arm64"
        arches.add(arch)
    return arches


def preferred_host_arch(sdk: dict, os_name: str) -> str:
    host_architectures = sdk.get("hostArchitectures", [])
    if os_name == "windows":
        for arch in ("x64", "x86", "arm64", "arm"):
            if arch in host_architectures:
                return arch
    elif os_name == "macos":
        for arch in ("x64", "arm64", "arm"):
            if arch in host_architectures:
                return arch
    elif os_name == "linux":
        for arch in ("x64", "arm64", "x86", "arm"):
            if arch in host_architectures:
                return arch
    return host_architectures[0] if host_architectures else ""


def matrix_host_arches(sdk: dict, os_name: str, macos_host_arches: set[str] | None = None) -> list[str]:
    host_architectures = sdk.get("hostArchitectures", [])
    if os_name != "macos":
        host_arch = preferred_host_arch(sdk, os_name)
        return [host_arch] if host_arch else []

    arches = []
    for arch in ("x64", "arm64", "arm"):
        if arch not in host_architectures:
            continue
        normalized = "arm64" if arch == "arm" else arch
        if macos_host_arches and normalized not in macos_host_arches:
            continue
        if normalized not in arches:
            arches.append(normalized)
    if arches:
        return arches
    if macos_host_arches:
        return []

    host_arch = preferred_host_arch(sdk, os_name)
    return [host_arch] if host_arch else []


def cmake_platform_for(os_name: str, host_arch: str) -> str:
    if os_name != "windows":
        return ""
    return {
        "x86": "Win32",
        "x64": "x64",
        "arm64": "ARM64",
        "arm": "ARM",
    }.get(host_arch, "")


def cmake_osx_arch_for(os_name: str, host_arch: str) -> str:
    if os_name != "macos":
        return ""
    return {
        "x64": "x86_64",
        "arm64": "arm64",
        "arm": "arm64",
    }.get(host_arch, "")


def package_ea_suffix(sdk: dict) -> str:
    return "-".join(f"ea{mode}" for mode in sdk.get("eaModes", []))


def package_asset_name(sdk: dict, os_name: str, host_arch: str) -> str:
    return f"ghidradec-ida{sdk['version']}-{os_name}-{host_arch}-{package_ea_suffix(sdk)}.zip"


def selected_matrix_entries(
    manifest: dict,
    *,
    versions: set[str],
    all_versions: bool,
    include_private: bool,
    current_host: bool,
    include_public: bool = True,
    macos_host_arches: set[str] | None = None,
) -> list[dict]:
    if all_versions and versions:
        raise SystemExit("--all and --versions are mutually exclusive")

    known_versions = {sdk["version"] for sdk in manifest["idaSdks"]}
    versions = expand_version_selectors(versions, known_versions)

    current = host_platform()
    default_version = manifest["project"]["defaultIdaSdk"]
    include = []
    for sdk in manifest["idaSdks"]:
        if not sdk.get("enabled", False):
            continue
        is_public = sdk["source"] in PUBLIC_SDK_SOURCES
        if not include_public and is_public:
            continue
        if versions:
            if sdk["version"] not in versions:
                continue
        elif not all_versions:
            if sdk["version"] != default_version:
                continue
        if not is_public and not include_private:
            continue
        for os_name in sdk.get("platforms", ["windows", "linux", "macos"]):
            if current_host and os_name != current:
                continue
            runner = {
                "windows": "windows-latest",
                "linux": "ubuntu-latest",
                "macos": "macos-latest",
            }[os_name]
            for host_arch in matrix_host_arches(sdk, os_name, macos_host_arches):
                include.append(
                    {
                        "ida": sdk["version"],
                        "os": os_name,
                        "runner": runner,
                        "sdk_dir": sdk["directory"],
                        "sdk_source": sdk["source"],
                        "cmake_var": sdk["cmakeVariable"],
                        "host_arch": host_arch,
                        "cmake_platform": cmake_platform_for(os_name, host_arch),
                        "cmake_osx_arch": cmake_osx_arch_for(os_name, host_arch),
                        "asset_name": package_asset_name(sdk, os_name, host_arch),
                    }
                )
    return include


def load_existing_assets(path: str | None) -> set[str]:
    if not path:
        return set()
    asset_path = Path(path)
    if not asset_path.exists():
        return set()
    return {line.strip() for line in asset_path.read_text(encoding="utf-8").splitlines() if line.strip()}


def has_existing_release_asset(entry: dict, existing_assets: set[str]) -> bool:
    return entry.get("asset_name", "") in existing_assets


def extract_archive(archive: Path, destination_root: Path, directory: str) -> Path:
    if not archive.exists():
        raise SystemExit(f"Archive not found: {archive}")

    destination_root.mkdir(parents=True, exist_ok=True)
    sdk_dir = destination_root / directory
    if sdk_dir.exists() and (sdk_dir / "include" / "ida.hpp").exists():
        return sdk_dir

    scratch = destination_root / f".extract-{directory}"
    if scratch.exists():
        shutil.rmtree(scratch)
    scratch.mkdir(parents=True)

    with zipfile.ZipFile(archive) as zf:
        zf.extractall(scratch)

    nested_archive = scratch / f"{directory}.zip"
    if not any(scratch.rglob("include/ida.hpp")) and nested_archive.exists():
        nested_scratch = scratch / ".nested"
        nested_scratch.mkdir(parents=True)
        with zipfile.ZipFile(nested_archive) as zf:
            zf.extractall(nested_scratch)
        for child in list(scratch.iterdir()):
            if child != nested_scratch:
                if child.is_dir():
                    shutil.rmtree(child)
                else:
                    child.unlink()
        for child in nested_scratch.iterdir():
            shutil.move(str(child), str(scratch / child.name))
        shutil.rmtree(nested_scratch, ignore_errors=True)

    source_dir = scratch
    for marker in scratch.rglob("include/ida.hpp"):
        source_dir = marker.parent.parent
        break
    if not (source_dir / "include" / "ida.hpp").exists():
        raise SystemExit(f"{archive} did not extract to a recognizable IDA SDK layout")

    if sdk_dir.exists():
        shutil.rmtree(sdk_dir)
    shutil.move(str(source_dir), str(sdk_dir))
    shutil.rmtree(scratch, ignore_errors=True)
    return sdk_dir


def download(url: str, output: Path) -> Path:
    output.parent.mkdir(parents=True, exist_ok=True)
    if output.exists():
        return output
    print(f"Downloading {url}")
    with urllib.request.urlopen(url) as response, output.open("wb") as handle:
        shutil.copyfileobj(response, handle)
    return output


def private_archive_url(base_url: str, archive_name: str) -> str:
    if not base_url:
        raise SystemExit("A private SDK base URL is required for this SDK.")
    return base_url.rstrip("/") + "/" + urllib.parse.quote(archive_name)


def retry_after_seconds(exc: urllib.error.HTTPError, default_delay: int) -> int:
    value = exc.headers.get("Retry-After", "") if exc.headers else ""
    if value:
        try:
            return max(1, int(value))
        except ValueError:
            pass
    return default_delay


def download_private_archive(
    base_url: str,
    token: str,
    archive_name: str,
    output: Path,
    *,
    retries: int = 8,
    retry_delay: int = 30,
) -> Path:
    output.parent.mkdir(parents=True, exist_ok=True)
    if output.exists():
        return output

    url = private_archive_url(base_url, archive_name)
    temporary_output = output.with_name(output.name + ".tmp")
    if temporary_output.exists():
        temporary_output.unlink()
    attempts = retries + 1
    for attempt in range(1, attempts + 1):
        request = urllib.request.Request(url)
        if token:
            request.add_header("Authorization", f"Bearer {token}")
            request.add_header("X-GhidraDec-Token", token)

        print(f"Downloading private SDK archive {archive_name} (attempt {attempt}/{attempts})")
        try:
            with urllib.request.urlopen(request, timeout=120) as response, temporary_output.open("wb") as handle:
                shutil.copyfileobj(response, handle)
            temporary_output.replace(output)
            return output
        except urllib.error.HTTPError as exc:
            temporary_output.unlink(missing_ok=True)
            if exc.code in {429, 500, 502, 503, 504} and attempt < attempts:
                delay = retry_after_seconds(exc, retry_delay)
                print(f"Private SDK fetch got HTTP {exc.code}; retrying in {delay} seconds.")
                time.sleep(delay)
                retry_delay = min(retry_delay * 2, 300)
                continue
            raise SystemExit(
                f"Private SDK fetch failed for {archive_name}: HTTP {exc.code}. "
                "Check the temporary URL, token, and archive name."
            ) from exc
        except urllib.error.URLError as exc:
            temporary_output.unlink(missing_ok=True)
            if attempt < attempts:
                print(f"Private SDK fetch failed for {archive_name}: {exc.reason}; retrying in {retry_delay} seconds.")
                time.sleep(retry_delay)
                retry_delay = min(retry_delay * 2, 300)
                continue
            raise SystemExit(
                f"Private SDK fetch failed for {archive_name}: {exc.reason}. "
                "Check that the temporary SDK server/tunnel is reachable."
            ) from exc
        except OSError as exc:
            temporary_output.unlink(missing_ok=True)
            if attempt < attempts:
                print(f"Private SDK fetch failed for {archive_name}: {exc}; retrying in {retry_delay} seconds.")
                time.sleep(retry_delay)
                retry_delay = min(retry_delay * 2, 300)
                continue
            raise SystemExit(f"Private SDK fetch failed for {archive_name}: {exc}") from exc
    raise SystemExit(f"Private SDK fetch failed for {archive_name}: retry limit exceeded.")


def fetch_private_sdk(args: argparse.Namespace) -> None:
    manifest = load_manifest()
    sdk = find_sdk(manifest, args.version or manifest["project"]["defaultIdaSdk"])
    if sdk["source"] in PUBLIC_SDK_SOURCES and not args.force:
        print(f"IDA SDK {sdk['version']} is public; use `ensure` unless you intentionally pass --force.")
        return

    destination_root = Path(args.destination).resolve() if args.destination else default_sdk_dir()
    base_url = args.base_url or os.environ.get("GHIDRADEC_PRIVATE_SDK_BASE_URL", "")
    token = args.token or os.environ.get("GHIDRADEC_PRIVATE_SDK_TOKEN", "")
    retries = args.retries if args.retries is not None else env_int("GHIDRADEC_PRIVATE_SDK_FETCH_RETRIES", 8)
    retry_delay = args.retry_delay if args.retry_delay is not None else env_int("GHIDRADEC_PRIVATE_SDK_FETCH_RETRY_DELAY", 30)
    archive = download_private_archive(
        base_url,
        token,
        sdk["archive"],
        destination_root / sdk["archive"],
        retries=max(0, retries),
        retry_delay=max(1, retry_delay),
    )
    print(archive)


def ensure_sdk(args: argparse.Namespace) -> None:
    manifest = load_manifest()
    sdk = find_sdk(manifest, args.version or manifest["project"]["defaultIdaSdk"])
    destination_root = Path(args.destination).resolve() if args.destination else default_sdk_dir()

    if sdk["source"] in PUBLIC_SDK_SOURCES:
        archive = download(sdk["url"], destination_root / sdk["archive"])
    else:
        archive = destination_root / sdk["archive"]
        if not archive.exists():
            raise SystemExit(
                f"Private SDK archive missing: {archive}. "
                f"Place {sdk['archive']} in {destination_root} or pass --destination."
            )

    sdk_dir = extract_archive(archive, destination_root, sdk["directory"])
    print(sdk_dir)


def matrix(args: argparse.Namespace) -> None:
    manifest = load_manifest()
    include_private = args.include_private or os.environ.get("GHIDRADEC_INCLUDE_PRIVATE_SDKS") == "1"
    include = selected_matrix_entries(
        manifest,
        versions=parse_versions(args.versions),
        all_versions=args.all,
        include_private=include_private,
        current_host=args.current_host,
        macos_host_arches=parse_arches(args.macos_host_arches),
    )
    existing_assets = load_existing_assets(args.existing_assets_file)
    if existing_assets and not args.rebuild_existing:
        skipped = []
        kept = []
        for entry in include:
            if has_existing_release_asset(entry, existing_assets):
                skipped.append(entry)
            else:
                kept.append(entry)
        for entry in skipped:
            print(
                f"Skipping IDA {entry['ida']} {entry['os']}; release already has a matching asset.",
                file=sys.stderr,
            )
        include = kept
    if args.require_nonempty and not include:
        raise SystemExit("No enabled IDA SDK targets matched the requested matrix.")
    print(json.dumps({"include": include}, separators=(",", ":")))


def release_asset_names(gh: str, tag: str, dry_run: bool) -> set[str]:
    if dry_run:
        return set()
    output = capture(
        [
            gh,
            "release",
            "view",
            tag,
            "--json",
            "assets",
            "--jq",
            ".assets[].name",
        ],
        dry_run=dry_run,
    )
    return {line.strip() for line in output.splitlines() if line.strip()}


def build_package(entry: dict, args: argparse.Namespace) -> list[Path]:
    version = entry["ida"]
    build_dir = Path(args.build_root) / f"ida-{version}"
    build_command = [
        sys.executable,
        "tools/build.py",
        "--ida-version",
        version,
        "--build-dir",
        str(build_dir),
        "--config",
        args.config,
    ]
    if args.generator:
        build_command.extend(["--generator", args.generator])
    if entry.get("cmake_platform"):
        build_command.extend(["--platform", entry["cmake_platform"]])
    if entry.get("cmake_osx_arch"):
        build_command.extend(["--osx-arch", entry["cmake_osx_arch"]])
    run(build_command, dry_run=args.dry_run)

    cmake = args.cmake or cmake_command()
    run(
        [
            cmake,
            "--build",
            str(build_dir),
            "--config",
            args.config,
            "--target",
            "package_plugins",
        ],
        dry_run=args.dry_run,
    )
    if args.dry_run:
        return []

    artifacts_dir = ROOT / build_dir / "artifacts"
    artifacts = sorted(artifacts_dir.glob("*.zip"))
    if not artifacts:
        raise SystemExit(f"No package ZIPs were produced under {artifacts_dir}")
    return artifacts


def upload_assets(gh: str, tag: str, artifacts: list[Path], existing: set[str], dry_run: bool) -> None:
    missing = [artifact for artifact in artifacts if artifact.name not in existing]
    for artifact in artifacts:
        if artifact.name in existing:
            print(f"Skipping existing release asset: {artifact.name}")
    if not missing:
        return
    run([gh, "release", "upload", tag, *[str(path) for path in missing]], dry_run=dry_run)
    existing.update(path.name for path in missing)


def release_backfill(args: argparse.Namespace) -> None:
    manifest = load_manifest()
    versions = parse_versions(args.versions)
    all_versions = args.all or not versions
    include = selected_matrix_entries(
        manifest,
        versions=versions,
        all_versions=all_versions,
        include_private=True,
        current_host=True,
        include_public=args.include_public,
        macos_host_arches=parse_arches(args.macos_host_arches),
    )
    include = [entry for entry in include if args.include_public or entry["sdk_source"] not in PUBLIC_SDK_SOURCES]
    if not include:
        raise SystemExit("No current-host release backfill targets matched the requested versions.")

    existing = release_asset_names(args.gh, args.tag, args.dry_run) if args.upload else set()
    for entry in include:
        artifacts = build_package(entry, args)
        if args.upload:
            upload_assets(args.gh, args.tag, artifacts, existing, args.dry_run)
        else:
            for artifact in artifacts:
                print(artifact)


def main() -> int:
    parser = argparse.ArgumentParser()
    sub = parser.add_subparsers(dest="command", required=True)

    ensure = sub.add_parser("ensure", help="Download/extract a target IDA SDK")
    ensure.add_argument("--version")
    ensure.add_argument("--destination")
    ensure.set_defaults(func=ensure_sdk)

    fetch_private = sub.add_parser(
        "fetch-private",
        help="Download a private/local SDK archive from a temporary authenticated URL.",
    )
    fetch_private.add_argument("--version")
    fetch_private.add_argument("--destination")
    fetch_private.add_argument("--base-url", default="", help="Temporary base URL; may already include a path token.")
    fetch_private.add_argument("--token", default="", help="Optional bearer token. GHIDRADEC_PRIVATE_SDK_TOKEN is also honored.")
    fetch_private.add_argument("--retries", type=int, default=None, help="Private SDK fetch retry count. Defaults to GHIDRADEC_PRIVATE_SDK_FETCH_RETRIES or 8.")
    fetch_private.add_argument("--retry-delay", type=int, default=None, help="Initial retry delay in seconds. Defaults to GHIDRADEC_PRIVATE_SDK_FETCH_RETRY_DELAY or 30.")
    fetch_private.add_argument("--force", action="store_true", help="Allow fetching SDKs marked public in the manifest.")
    fetch_private.set_defaults(func=fetch_private_sdk)

    mat = sub.add_parser("matrix", help="Emit GitHub Actions matrix JSON")
    mat.add_argument("--include-private", action="store_true")
    mat.add_argument("--current-host", action="store_true")
    mat.add_argument("--all", action="store_true", help="Include every enabled SDK target.")
    mat.add_argument("--versions", help="Comma-separated SDK versions to include.")
    mat.add_argument("--require-nonempty", action="store_true", help="Fail if the selected matrix is empty.")
    mat.add_argument("--existing-assets-file", help="Optional newline-delimited release asset names to skip.")
    mat.add_argument("--rebuild-existing", action="store_true", help="Do not skip matrix entries with existing release assets.")
    mat.add_argument("--macos-host-arches", default="", help="Comma-separated macOS host arches to emit: x64, arm64, or all. Defaults to all supported.")
    mat.set_defaults(func=matrix)

    backfill = sub.add_parser(
        "release-backfill",
        help="Build current-host local SDK packages and upload missing assets to a GitHub Release.",
    )
    backfill.add_argument("--tag", required=True, help="Existing GitHub Release tag, for example v1.0.")
    backfill.add_argument(
        "--versions",
        help="Comma-separated SDK versions to build. Defaults to all private/local current-host SDKs.",
    )
    backfill.add_argument("--all", action="store_true", help="Build every matching current-host SDK target.")
    backfill.add_argument(
        "--include-public",
        action="store_true",
        help="Also build public SDK targets such as 9.2, 9.3, and latest.",
    )
    backfill.add_argument("--config", default="Release")
    backfill.add_argument("--generator", default="")
    backfill.add_argument("--build-root", default=str(Path("build") / "matrix"))
    backfill.add_argument("--cmake", default="", help="CMake executable override.")
    backfill.add_argument("--macos-host-arches", default="", help="Comma-separated macOS host arches to build: x64, arm64, or all. Defaults to all supported.")
    backfill.add_argument("--gh", default="gh", help="GitHub CLI executable.")
    backfill.add_argument(
        "--no-upload",
        dest="upload",
        action="store_false",
        help="Build packages but do not upload them.",
    )
    backfill.add_argument(
        "--rebuild-existing",
        dest="skip_existing",
        action="store_false",
        help="Build even when a matching release asset already exists.",
    )
    backfill.add_argument("--dry-run", action="store_true", help="Print commands without running them.")
    backfill.set_defaults(func=release_backfill, upload=True, skip_existing=True)

    args = parser.parse_args()
    args.func(args)
    return 0


if __name__ == "__main__":
    sys.exit(main())
