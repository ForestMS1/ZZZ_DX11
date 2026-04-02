xcopy /y /I ".\Engine\Public\*.h" ".\Libraries\Include\EngineSDK"
xcopy /y /I ".\Engine\Public\*.inl" ".\Libraries\Include\EngineSDK"

xcopy /y /I ".\Engine\Bin\Engine.idb" ".\Libraries\Lib\EngineSDK"
xcopy /y /I ".\Engine\Bin\Engine.lib" ".\Libraries\Lib\EngineSDK"
xcopy /y /I ".\Engine\Bin\Engine.pdb" ".\Libraries\Lib\EngineSDK"
xcopy /y /I ".\Engine\Bin\Engine.dll" ".\Libraries\Lib\EngineSDK"

xcopy /y /I ".\Engine\Bin\Engine.lib" ".\Client\Bin"
xcopy /y /I ".\Engine\Bin\Engine.dll" ".\Client\Bin"
xcopy /y /I ".\Engine\Bin\Engine.lib" ".\AssimpTool\Bin"
xcopy /y /I ".\Engine\Bin\Engine.dll" ".\AssimpTool\Bin"

exit /b 0