# build_server.ps1 — Compila e executa o servidor COPA VISION 2026
# Uso: cd COPAVISION; .\build_server.ps1

Write-Host "`n=============================================" -ForegroundColor Cyan
Write-Host "   COPA VISION 2026 — Build & Run Server   " -ForegroundColor Cyan
Write-Host "=============================================`n" -ForegroundColor Cyan

# Verificar httplib.h
if (-not (Test-Path "httplib.h")) {
    Write-Host "[1/3] Baixando httplib.h..." -ForegroundColor Yellow
    try {
        Invoke-WebRequest -Uri "https://raw.githubusercontent.com/yhirose/cpp-httplib/master/httplib.h" `
                          -OutFile "httplib.h" -UseBasicParsing
        Write-Host "      httplib.h baixado!" -ForegroundColor Green
    } catch {
        Write-Host "[ERRO] Nao foi possivel baixar httplib.h." -ForegroundColor Red
        exit 1
    }
} else {
    Write-Host "[1/3] httplib.h ja presente. OK" -ForegroundColor Green
}

# Compilar
Write-Host "[2/3] Compilando server.cpp com g++..." -ForegroundColor Yellow
$args_cpp = @(
    "-std=c++17",
    "-O2",
    "server.cpp",
    "SelecoesLoader.cpp",
    "DadosHistoricos.cpp",
    "ConsoleUtils.cpp",
    "-o", "server.exe",
    "-lws2_32"
)

& g++ @args_cpp

if ($LASTEXITCODE -ne 0) {
    Write-Host "[ERRO] Compilacao falhou. Verifique as mensagens acima." -ForegroundColor Red
    exit 1
}
Write-Host "      Compilado com sucesso -> server.exe" -ForegroundColor Green

# Executar
Write-Host "[3/3] Iniciando servidor em http://localhost:8080 ..." -ForegroundColor Yellow
Write-Host "      (Mantenha este terminal aberto. Use Ctrl+C para parar)`n" -ForegroundColor Gray
.\server.exe
