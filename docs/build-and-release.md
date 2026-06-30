# Build and Release Workflow

`ghidradec.targets.json` is the source of truth for supported IDA SDKs,
platforms, EA modes, and source dependency artifacts.

Public IDA SDKs, currently 9.2, 9.3, and `latest`, are downloaded from the
Hex-Rays `ida-sdk` GitHub repository. `latest` follows the repository's master
branch and is intentionally moving. Older SDKs must stay private: put local
archives in `.idasdks`, encrypt them into `idasdks/*.enc`, and publish only
those encrypted files.

## Local Build

```bash
python tools/build.py --ida-version 9.3
python tools/build.py --ida-version latest
```

The helper downloads/extracts public SDKs into `.idasdks` and then configures
CMake with `GHIDRADEC_IDA_VERSION`.

## Private SDK Encryption

```bash
export IDA_SDK_AES_KEY="<strong shared secret>"
python tools/ida_sdk.py encrypt \
  --input .idasdks/idasdk68.zip \
  --output idasdks/idasdk68.zip.enc
python tools/ida_sdk.py decrypt-private
```

In GitHub Actions, store the same value as the `IDA_SDK_AES_KEY` secret. The CI
workflow uses `python tools/ida_sdk.py decrypt-private` to decrypt
`idasdks/*.enc` into `.idasdks` before building private SDK targets. The Python
command uses OpenSSL for AES-CBC, so OpenSSL must be available on the runner.

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

The release workflow only runs on tagged refs. For an on-demand backfill, open
Actions, choose the Release workflow, select the official release tag in the
branch/tag picker, and set `ida_versions` to the missing SDK version or list.
The publish step uploads only release ZIPs that are not already attached to that
GitHub Release, so older SDK packages can be generated once and then remain
available for everyone.
