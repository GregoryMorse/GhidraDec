param(
    [Parameter(Mandatory = $true)]
    [ValidateSet("encrypt", "decrypt")]
    [string]$Action,

    [Parameter(Mandatory = $true)]
    [string]$InputPath,

    [Parameter(Mandatory = $true)]
    [string]$OutputPath,

    [string]$Password = $env:IDA_SDK_AES_KEY
)

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

if ([string]::IsNullOrWhiteSpace($Password)) {
    throw "Password was not provided. Pass -Password or set IDA_SDK_AES_KEY."
}

$magic = [Text.Encoding]::ASCII.GetBytes("GDECSDK1")
$iterations = 200000

function New-RandomBytes([int]$Count) {
    $bytes = [byte[]]::new($Count)
    $rng = [Security.Cryptography.RandomNumberGenerator]::Create()
    try {
        $rng.GetBytes($bytes)
    }
    finally {
        $rng.Dispose()
    }
    return $bytes
}

function Test-FixedTimeEquals([byte[]]$Left, [byte[]]$Right) {
    if ($Left.Length -ne $Right.Length) {
        return $false
    }
    $diff = 0
    for ($i = 0; $i -lt $Left.Length; $i++) {
        $diff = $diff -bor ($Left[$i] -bxor $Right[$i])
    }
    return $diff -eq 0
}

function New-KeyMaterial([byte[]]$Salt, [string]$Secret) {
    $derive = [Security.Cryptography.Rfc2898DeriveBytes]::new(
        $Secret,
        $Salt,
        $iterations,
        [Security.Cryptography.HashAlgorithmName]::SHA256)
    try {
        return $derive.GetBytes(64)
    }
    finally {
        $derive.Dispose()
    }
}

function Protect-Bytes([byte[]]$Plaintext, [byte[]]$Key, [byte[]]$Iv) {
    $aes = [Security.Cryptography.Aes]::Create()
    try {
        $aes.Mode = [Security.Cryptography.CipherMode]::CBC
        $aes.Padding = [Security.Cryptography.PaddingMode]::PKCS7
        $aes.Key = $Key
        $aes.IV = $Iv
        $encryptor = $aes.CreateEncryptor()
        try {
            return $encryptor.TransformFinalBlock($Plaintext, 0, $Plaintext.Length)
        }
        finally {
            $encryptor.Dispose()
        }
    }
    finally {
        $aes.Dispose()
    }
}

function Unprotect-Bytes([byte[]]$Ciphertext, [byte[]]$Key, [byte[]]$Iv) {
    $aes = [Security.Cryptography.Aes]::Create()
    try {
        $aes.Mode = [Security.Cryptography.CipherMode]::CBC
        $aes.Padding = [Security.Cryptography.PaddingMode]::PKCS7
        $aes.Key = $Key
        $aes.IV = $Iv
        $decryptor = $aes.CreateDecryptor()
        try {
            return $decryptor.TransformFinalBlock($Ciphertext, 0, $Ciphertext.Length)
        }
        finally {
            $decryptor.Dispose()
        }
    }
    finally {
        $aes.Dispose()
    }
}

function New-Hmac([byte[]]$Key, [byte[]]$Data) {
    $hmac = [Security.Cryptography.HMACSHA256]::new($Key)
    try {
        return $hmac.ComputeHash($Data)
    }
    finally {
        $hmac.Dispose()
    }
}

function Join-Bytes([byte[][]]$Arrays) {
    $length = 0
    foreach ($array in $Arrays) {
        $length += $array.Length
    }
    $result = [byte[]]::new($length)
    $offset = 0
    foreach ($array in $Arrays) {
        [Array]::Copy($array, 0, $result, $offset, $array.Length)
        $offset += $array.Length
    }
    return $result
}

$inputBytes = [IO.File]::ReadAllBytes((Resolve-Path -LiteralPath $InputPath))
$outputParent = Split-Path -Parent $OutputPath
if ($outputParent) {
    New-Item -ItemType Directory -Force -Path $outputParent | Out-Null
}

if ($Action -eq "encrypt") {
    $salt = New-RandomBytes 16
    $iv = New-RandomBytes 16
    $material = New-KeyMaterial $salt $Password
    $encKey = $material[0..31]
    $macKey = $material[32..63]
    $ciphertext = Protect-Bytes $inputBytes $encKey $iv
    $authenticated = Join-Bytes @($magic, $salt, $iv, $ciphertext)
    $tag = New-Hmac $macKey $authenticated
    [IO.File]::WriteAllBytes($OutputPath, (Join-Bytes @($authenticated, $tag)))
    return
}

if ($inputBytes.Length -lt ($magic.Length + 16 + 16 + 32)) {
    throw "Encrypted SDK file is too short."
}

for ($i = 0; $i -lt $magic.Length; $i++) {
    if ($inputBytes[$i] -ne $magic[$i]) {
        throw "Encrypted SDK file has an unknown format."
    }
}

$salt = $inputBytes[$magic.Length..($magic.Length + 15)]
$ivStart = $magic.Length + 16
$iv = $inputBytes[$ivStart..($ivStart + 15)]
$tagStart = $inputBytes.Length - 32
$cipherStart = $ivStart + 16
$ciphertext = $inputBytes[$cipherStart..($tagStart - 1)]
$tag = $inputBytes[$tagStart..($inputBytes.Length - 1)]

$material = New-KeyMaterial $salt $Password
$encKey = $material[0..31]
$macKey = $material[32..63]
$authenticated = $inputBytes[0..($tagStart - 1)]
$expected = New-Hmac $macKey $authenticated
if (-not (Test-FixedTimeEquals $tag $expected)) {
    throw "Encrypted SDK authentication failed."
}

[IO.File]::WriteAllBytes($OutputPath, (Unprotect-Bytes $ciphertext $encKey $iv))
