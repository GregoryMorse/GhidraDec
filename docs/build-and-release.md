# Build and Release Workflow

`ghidradec.targets.json` is the source of truth for supported IDA SDKs,
platforms, EA modes, and source dependency artifacts.

Public IDA SDKs, currently 9.2 and 9.3, are downloaded from the Hex-Rays
`ida-sdk` GitHub releases. Older SDKs must stay private: put local archives in
`.idasdks`, encrypt them into `idasdks/*.enc`, and publish only those encrypted
files.

## Local Build

```powershell
pwsh tools/build.ps1 -IdaVersion 9.3
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
python tools/ida_sdk.py matrix --include-private
```

By default, only enabled public targets are emitted. Use `--include-private`
when encrypted SDKs and the decryption secret are available.
