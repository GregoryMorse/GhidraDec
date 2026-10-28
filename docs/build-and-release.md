# Build and Release Workflow

`ghidradec.targets.json` is the source of truth for supported IDA SDKs,
platforms, EA modes, and source dependency artifacts.

Public IDA SDKs are downloaded from the Hex-Rays `ida-sdk` GitHub repository.
IDA 9.2 through 9.5, including the published SDK revisions, are pinned in the
manifest. Future public SDKs are generated from the Hex-Rays tag pattern, so
`--versions 9.6` resolves to `v9.6.0-release`. `latest`
follows the repository's master branch and is intentionally moving. Older SDKs
must stay private: place local archives in `.idasdks` or pass an explicit SDK
path with `IDA_SDK_DIR` / `IDA_SDK_DIR32`. Do not commit IDA SDK archives,
encrypted or otherwise.

Short revision selectors are supported in both Actions' `ida_versions` input
and local `--ida-version`/`--version` options:

| Input | Upstream Tag | Canonical Package Version |
| --- | --- | --- |
| `9.4` | `v9.4.0-release` | `9.4` |
| `9.4sp1` | `v9.4.0-sdk.1` | `9.4.0-sdk.1` |
| `9.3sp1` | `v9.3.1-sdk.1` | `9.3.1-sdk.1` |
| `9.5` | `v9.5.0-release` | `9.5` |

`spN` here is a **selector convenience for SDK revision N**, not a claim that
Hex-Rays released a product named IDA 9.4 SP1. An exact tag such as
`v9.4.0-sdk.1` is also accepted. Future `9.6sp2` follows
`v9.6.0-sdk.2`; add a manifest alias when upstream changes the patch number,
as it did for 9.3. Inspect a resolution without downloading:

```bash
python tools/ida_sdk.py describe --version 9.4sp1
python tools/ida_sdk.py matrix --versions 9.4,9.4sp1,9.5
```

Python 3.10+ is required. CMake invokes the same resolver as the matrix, so SDK
aliases cannot accidentally select a different directory or package name.

## Local Build

```bash
python tools/build.py --ida-version 9.5
python tools/build.py --ida-version 9.4sp1
python tools/build.py --ida-version latest
```

The helper downloads/extracts public SDKs into `.idasdks` and then configures
CMake with `GHIDRADEC_IDA_VERSION`.
It also provisions Ghidra 12.2 support sources. Generated subsets carry a
`source-version.json` marker: a 12.1 subset is not reused for a 12.2 build.
An old CMake build directory with a cached `GHIDRA_VER` must be reconfigured
with `-DGHIDRA_VER=12.2`; the build helper sets this explicitly.

An installed runtime that includes sources avoids downloading another archive:

```bash
python tools/deps.py ensure-ghidra-sources --source-dir /path/to/ghidra_12.2_PUBLIC/Ghidra/Features/Decompiler/src/decompile/cpp
```

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
IDA SDK 9.5. Use `--versions` for a comma-separated backfill set, future public
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

CI and manual release runs build the pinned public `9.5` SDK by default.
Tag pushes build all six pinned public SDK variants (9.2, 9.3, 9.3sp1, 9.4,
9.4sp1 and 9.5). Runs can set `ida_versions` to these versions, an exact
public tag, `latest`, a legacy SDK such as `7.7`, a major group such as `8`, a
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

Public SDKs and Ghidra support sources are cached by version/provisioner.
Public RetDec/jsoncpp download archives share a build-local cache as well.
`latest` is never restored from an immutable SDK cache.
**Private SDK archives and extracted trees are never put in Actions caches
or workflow artifacts**, because caches in a public repository must not become
a licensed-SDK distribution channel.

Manual release backfills run sequentially to keep temporary SDK tunnels gentle.
Automatic public-SDK tag builds use up to four concurrent jobs.

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

Microsoft Dev Tunnels can expose the same localhost server. Keep private
backfills deferred until the public-SDK staging matrix is green. An optional
local `.env.private-sdk` is ignored by Git; never commit tunnel credentials.
It is local operator configuration, not automatically sourced by the tools.

For a temporary tunnel compatible with the workflow's existing bearer-token
input, first run the archive server with a strong token in an ignored local
file (for example `build/private-sdk.token`):

```powershell
python tools/private_sdk_server.py --root .idasdks --port 8765 --token-file build/private-sdk.token
```

In another terminal, after completing the interactive login yourself:

```powershell
devtunnel user login
devtunnel host -p 8765 --protocol http --allow-anonymous
```

Here `--allow-anonymous` applies only to the tunnel relay. **The archive server
must still require its own token**; do not expose a tokenless SDK server. Use
the printed HTTPS port URL for `private_sdk_base_url` and the server's token
for `private_sdk_token`. Do not use the inspection URL. Stop both processes
after the backfill. A relay restricted with Dev Tunnel client tokens is also
possible but needs a separate `X-Tunnel-Authorization` header, which the current
workflow does not configure.

See Microsoft's [setup](https://learn.microsoft.com/en-us/azure/developer/dev-tunnels/get-started)
and [CLI reference](https://learn.microsoft.com/en-us/azure/developer/dev-tunnels/cli-commands).
Reuse public dependency caches and unchanged completed plugin artifacts before
spending more tunnel bandwidth. If a fix changes plugin sources, rebuild the
affected assets; retaining a tag must not silently retain stale binaries.

Private SDK downloads retry transient tunnel failures such as HTTP 429 and 5xx
responses. The workflow defaults are intentionally patient for long `all`
backfills. Locally, use `tools/ida_sdk.py fetch-private --retries N
--retry-delay SECONDS` or the `GHIDRADEC_PRIVATE_SDK_FETCH_RETRIES` /
`GHIDRADEC_PRIVATE_SDK_FETCH_RETRY_DELAY` environment variables to tune this.

Legacy SDK backfills should be built locally from licensed SDK archives and then
uploaded to the tagged GitHub Release, for example:

```bash
python tools/ida_sdk.py release-backfill --tag v1.1
```

By default, `release-backfill` builds every private/local SDK target for the
current host, skips any release asset that is already present, and uploads the
missing package ZIPs with the GitHub CLI. Use `--versions 7.7,8.5` for a smaller
set, `--include-public` to also rebuild public SDK assets, or `--no-upload` for
a packaging-only dry run.

This keeps permanent release assets available to everyone without redistributing
IDA SDK source material.

Each new GhidraDec tag needs newly compiled legacy plugin binaries. Do not copy
v1.0 plugin ZIPs into v1.1: they still embed the old Ghidra support sources.
The private set currently includes **6.x through 9.1**, not just pre-9.0.
To minimize reverse-tunnel data, build on a licensed local/self-hosted machine
with a persistent `GHIDRADEC_IDA_SDK_CACHE` and upload only the plugin ZIPs.
If using hosted runners instead, fetch each SDK on demand through the temporary
authenticated tunnel and stop it once all jobs finish. Do not leave the SDK
server exposed for future backfills.

SDK-free helper/policy checks are registered with CTest after configuration:

```bash
python -m unittest discover -s tests/unit -p "test_*.py"
ctest --test-dir build/matrix/ida-9.5 -C Release --output-on-failure
```

For licensed runtime selection and the difference between idat and idalib, see
[`ida-windows-test-grid.md`](ida-windows-test-grid.md).

Release package names include the IDA SDK version, target OS, linked IDA host
architecture, and included EA modes, for example
`ghidradec-ida7.7-windows-x64-ea32-ea64.zip` or
`ghidradec-ida9.3-linux-x64-ea64.zip`.
