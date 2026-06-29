# Build and Release Workflow

`ghidradec.targets.json` is the source of truth for supported IDA SDKs,
platforms, EA modes, and source dependency artifacts.

Public IDA SDKs, currently 9.2, 9.3, and `latest`, are downloaded from the
Hex-Rays `ida-sdk` GitHub repository. `latest` follows the repository's master
branch and is intentionally moving. Older SDKs must stay private: put local
archives in `.idasdks`, encrypt them into `idasdks/*.enc`, and publish only
those encrypted files.

## Local Build

```powershell
pwsh tools/build.ps1 -IdaVersion 9.3
pwsh tools/build.ps1 -IdaVersion latest
```

The helper downloads/extracts public SDKs into `.idasdks` and then configures
CMake with `GHIDRADEC_IDA_VERSION`.

## Private SDK Encryption

```powershell
$env:IDA_SDK_AES_KEY = "<strong shared secret>"
pwsh tools/ida_sdk_crypto.ps1 encrypt `
  -InputPath .idasdks/idasdk68.zip `
  -OutputPath idasdks/idasdk68.zip.enc
```

In GitHub Actions, store the same value as the `IDA_SDK_AES_KEY` secret. The CI
workflow decrypts `idasdks/*.enc` into `.idasdks` before building private SDK
targets.

## Target Matrix

```bash
python tools/ida_sdk.py matrix
python tools/ida_sdk.py matrix --versions 9.3
python tools/ida_sdk.py matrix --versions 7.3,8.5 --include-private
python tools/ida_sdk.py matrix --all --include-private
```

By default, the matrix emits only the manifest's `defaultIdaSdk`, currently
IDA SDK 9.3. Use `--versions` for a comma-separated backfill set, or `--all`
for every enabled manifest target. Use `--include-private` when encrypted SDKs
and the decryption secret are available.

## GitHub Actions

Normal CI and tag releases build only IDA SDK 9.3 by default. Manual workflow
runs can set `ida_versions` to a single version, a comma-separated list such as
`7.3,7.4,8.5`, `latest`, or `all`.

The release workflow is intended for tagged refs. When manually run on an
existing tag, it uploads only release ZIPs that are not already attached to that
GitHub Release, so older SDK packages can be generated on demand and then remain
available for everyone.
