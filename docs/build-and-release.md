# Build and Release Workflow

`ghidradec.targets.json` is the source of truth for supported IDA SDKs,
platforms, EA modes, and source dependency artifacts.

Public IDA SDKs are downloaded from the Hex-Rays `ida-sdk` GitHub repository.
IDA 9.2 and 9.3 are pinned in the manifest. IDA 9.4 and newer public release
SDKs are generated from the Hex-Rays tag pattern, so `--versions 9.4` resolves
to `v9.4.0-release`, `9.5` resolves to `v9.5.0-release`, and so on. `latest`
follows the repository's master branch and is intentionally moving. Older SDKs
must stay private: place local archives in `.idasdks` or pass an explicit SDK
path with `IDA_SDK_DIR` / `IDA_SDK_DIR32`. Do not commit IDA SDK archives,
encrypted or otherwise.

## Local Build

```bash
python tools/build.py --ida-version 9.3
python tools/build.py --ida-version latest
```

The helper downloads/extracts public SDKs into `.idasdks` and then configures
CMake with `GHIDRADEC_IDA_VERSION`.

## Legacy SDK Builds

```bash
python tools/ida_sdk.py ensure --version 7.7 --destination .idasdks
python tools/build.py --ida-version 7.7
python tools/ida_sdk.py matrix --versions 7.7 --include-private --current-host
```

For legacy SDK versions, put the licensed archive named in
`ghidradec.targets.json` under `.idasdks` first. For example, IDA 7.7 expects
`.idasdks/idasdk77.zip`; `tools/ida_sdk.py ensure --version 7.7` extracts it
to `.idasdks/idasdk77`.

Set `GHIDRADEC_IDA_SDK_CACHE` to keep those archives and extracted SDKs outside
the repo checkout; CMake will search that cache in addition to `.idasdks`.

These builds should be done locally or on a temporary private/self-hosted
runner. The resulting package ZIPs are the only artifacts that should be
published.

## Target Matrix

```bash
python tools/ida_sdk.py matrix
python tools/ida_sdk.py matrix --versions 9.3
python tools/ida_sdk.py matrix --versions 8 --include-private
python tools/ida_sdk.py matrix --versions 7.3,8.5 --include-private
python tools/ida_sdk.py matrix --all --include-private
```

By default, the matrix emits only the manifest's `defaultIdaSdk`, currently
IDA SDK 9.3. Use `--versions` for a comma-separated backfill set, future public
tags such as `9.4`, major groups `6`, `7`, `8`, or `9`, or `--all` for every
enabled manifest target. Use
`--include-private` only for local or private-runner builds where the required
licensed SDK archives are already available.

For macOS, the matrix emits both `x64` and `arm64` package rows when the SDK
provides both host library sets. Older SDKs remain `x64`-only. Windows and
Linux currently use the preferred hosted-runner architecture from the manifest.
Set the Release workflow's `macos_host_arches` input to `x64` or `arm64` to
limit a backfill run when you do not want both macOS packages.

## GitHub Actions

Manual CI and release runs build the moving public `latest` SDK by default.
Runs can set `ida_versions` to `9.2`, `9.3`, `9.4`, future public release SDKs
such as `9.5`, a legacy SDK such as `7.7`, a major group such as `8`, a
comma-separated list, or `all`.

The release workflow only runs on tagged refs. For an on-demand backfill, open
Actions, choose the Release workflow, select the official release tag in the
branch/tag picker, and set `ida_versions` to the missing public SDK version or
list. The publish step uploads only release ZIPs that are not already attached
to that GitHub Release.

For a package-only rehearsal, run the same workflow manually from a branch. It
builds the matrix and stores workflow artifacts, but the GitHub Release publish
job is skipped unless the selected ref is an official tag.

When running on a tag, the matrix skips version/OS entries that already have a
matching release ZIP. This makes `all` useful for backfilling only missing
assets. Set `rebuild_existing_assets` only when you have intentionally deleted
old assets or want to regenerate them.

`latest` is the exception because it follows a moving branch. Release runs
always rebuild `latest`, and the publish job replaces existing
`ghidradec-idalatest-*` assets with `gh release upload --clobber`.

The release build matrix runs sequentially. This keeps `all` backfills gentle on
temporary SDK tunnels and avoids many GitHub runners downloading private SDK
archives at the same time.

### Temporary Private SDK URL

Legacy SDKs can be built on GitHub-hosted Windows, Linux, and macOS runners
without committing or permanently hosting the SDK archives. Run a short-lived
local archive server, expose it with a temporary HTTPS tunnel, then paste that
URL into the Release workflow's `private_sdk_base_url` input.

```bash
python tools/private_sdk_server.py --root .idasdks --port 8765 --token <short-lived-token>
```

If the tunnel gives you `https://example-tunnel.trycloudflare.com`, either set:

```text
private_sdk_base_url = https://example-tunnel.trycloudflare.com/<short-lived-token>
private_sdk_token    =
```

or keep the token out of the URL path and use bearer auth:

```text
private_sdk_base_url = https://example-tunnel.trycloudflare.com
private_sdk_token    = <short-lived-token>
```

The workflow masks both fields in logs before downloading. If a private SDK is
requested without a reachable `private_sdk_base_url`, the build fails with a
clear setup error instead of silently skipping the asset.

Private SDK downloads retry transient tunnel failures such as HTTP 429 and 5xx
responses. The workflow defaults are intentionally patient for long `all`
backfills. Locally, use `tools/ida_sdk.py fetch-private --retries N
--retry-delay SECONDS` or the `GHIDRADEC_PRIVATE_SDK_FETCH_RETRIES` /
`GHIDRADEC_PRIVATE_SDK_FETCH_RETRY_DELAY` environment variables to tune this.

Legacy SDK backfills should be built locally from licensed SDK archives and then
uploaded to the tagged GitHub Release, for example:

```bash
python tools/ida_sdk.py release-backfill --tag v1.0
```

By default, `release-backfill` builds every private/local SDK target for the
current host, skips any release asset that is already present, and uploads the
missing package ZIPs with the GitHub CLI. Use `--versions 7.7,8.5` for a smaller
set, `--include-public` to also rebuild public SDK assets, or `--no-upload` for
a packaging-only dry run.

This keeps permanent release assets available to everyone without redistributing
IDA SDK source material.

Release package names include the IDA SDK version, target OS, linked IDA host
architecture, and included EA modes, for example
`ghidradec-ida7.7-windows-x64-ea32-ea64.zip` or
`ghidradec-ida9.3-linux-x64-ea64.zip`.
