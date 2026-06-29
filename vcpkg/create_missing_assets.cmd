@echo off

for /f "usebackq tokens=1-3 delims=;" %%A in ("missing-assets.txt") do (
    call asset-provider.bat %%A %%B %%C
)