#!/usr/bin/env python3
"""Materialize selected public binary corpora for IDA regression runs."""

from __future__ import annotations

import argparse
import json
import sys
import urllib.request
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
MANIFEST = ROOT / "ghidradec.corpus.json"
DEFAULT_DESTINATION = ROOT / "build" / "corpus"


def load_manifest(path: Path = MANIFEST) -> dict:
    return json.loads(path.read_text(encoding="utf-8"))


def selected_targets(manifest: dict, corpus_name: str, architectures: set[str], tiers: set[str]) -> list[tuple[dict, dict]]:
    orders = manifest.get("orders", {})
    order_names = orders.get("baseline-expansion") or orders.get("x86-first", [])
    order = {name: index for index, name in enumerate(order_names)}
    selected: list[tuple[dict, dict]] = []
    for corpus in manifest["corpora"]:
        if corpus_name != "all" and corpus["name"] != corpus_name:
            continue
        for target in corpus.get("targets", []):
            if not target.get("enabled", False):
                continue
            if architectures and target["arch"] not in architectures:
                continue
            if tiers and target.get("tier", "") not in tiers:
                continue
            selected.append((corpus, target))
    selected.sort(key=lambda item: (order.get(item[1]["arch"], 1000), item[1]["id"]))
    return selected


def target_output_path(destination: Path, corpus: dict, target: dict) -> Path:
    source_name = Path(target["path"]).name
    return destination / corpus["name"] / target["arch"] / target["id"] / source_name


def download(url: str, output: Path, refresh: bool) -> Path:
    if output.exists() and not refresh:
        return output
    output.parent.mkdir(parents=True, exist_ok=True)
    print(f"[ghidradec-corpus] downloading {url}", file=sys.stderr)
    with urllib.request.urlopen(url) as response, output.open("wb") as handle:
        handle.write(response.read())
    return output


def stage(args: argparse.Namespace) -> None:
    manifest = load_manifest(Path(args.manifest).resolve())
    destination = Path(args.destination).resolve()
    architectures = {value.strip() for value in args.arch.split(",") if value.strip()} if args.arch else set()
    tiers = {value.strip() for value in args.tier.split(",") if value.strip()} if args.tier else set()
    targets = selected_targets(manifest, args.corpus, architectures, tiers)
    if not targets:
        raise SystemExit("No corpus targets matched the requested filters.")

    staged_paths = []
    for corpus, target in targets:
        if corpus["kind"] != "github-raw":
            raise SystemExit(f"Unsupported corpus kind: {corpus['kind']}")
        output = target_output_path(destination, corpus, target)
        url = corpus["baseUrl"].rstrip("/") + "/" + target["path"]
        staged_paths.append(download(url, output, args.refresh))

    if args.output_list:
        list_path = Path(args.output_list).resolve()
        list_path.parent.mkdir(parents=True, exist_ok=True)
        list_path.write_text("\n".join(str(path) for path in staged_paths) + "\n", encoding="utf-8")

    for path in staged_paths:
        print(path)


def list_targets(args: argparse.Namespace) -> None:
    manifest = load_manifest(Path(args.manifest).resolve())
    architectures = {value.strip() for value in args.arch.split(",") if value.strip()} if args.arch else set()
    tiers = {value.strip() for value in args.tier.split(",") if value.strip()} if args.tier else set()
    for corpus, target in selected_targets(manifest, args.corpus, architectures, tiers):
        print(f"{corpus['name']}\t{target['arch']}\t{target.get('tier', '')}\t{target['id']}\t{target['path']}")


def add_common_filters(parser: argparse.ArgumentParser) -> None:
    parser.add_argument("--manifest", default=str(MANIFEST))
    parser.add_argument("--corpus", default="angr-binaries")
    parser.add_argument("--arch", help="Comma-separated architecture filter, e.g. x86_64,x86_32")
    parser.add_argument("--tier", default="smoke", help="Comma-separated tier filter; default is smoke")


def main() -> int:
    parser = argparse.ArgumentParser()
    sub = parser.add_subparsers(dest="command", required=True)

    list_parser = sub.add_parser("list", help="List selected corpus targets")
    add_common_filters(list_parser)
    list_parser.set_defaults(func=list_targets)

    stage_parser = sub.add_parser("stage", help="Download selected corpus targets into build/corpus")
    add_common_filters(stage_parser)
    stage_parser.add_argument("--destination", default=str(DEFAULT_DESTINATION))
    stage_parser.add_argument("--output-list", help="Write staged paths to a newline-delimited file")
    stage_parser.add_argument("--refresh", action="store_true")
    stage_parser.set_defaults(func=stage)

    args = parser.parse_args()
    args.func(args)
    return 0


if __name__ == "__main__":
    sys.exit(main())
