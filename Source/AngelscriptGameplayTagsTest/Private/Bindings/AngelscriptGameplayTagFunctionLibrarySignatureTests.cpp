#include "Core/AngelscriptBinds.h"
#include "Core/AngelscriptEngine.h"
#include "Binds/Helper_FunctionSignature.h"
#include "FunctionLibraries/GameplayTagQueryMixinLibrary.h"
#include "AngelscriptTestUtilities.h"

#include "CQTest.h"
#include "Misc/ScopeExit.h"

#include "StartAngelscriptHeaders.h"
#include "source/as_scriptfunction.h"
#include "EndAngelscriptHeaders.h"

#if WITH_DEV_AUTOMATION_TESTS

class asIScriptGeneric;

namespace AngelscriptGameplayTags_FunctionLibrarySignatureTests_Private
{
	void CDECL NoOpGeneric(asIScriptGeneric* Generic)
	{
		(void)Generic;
	}

	void ResetIsolatedEnvironment()
	{
		DestroySharedTestEngine();
		FAngelscriptBinds::ResetBindState();

		if (FAngelscriptEngine::IsInitialized())
		{
			FAngelscriptTestEngineScopeAccess::DestroyGlobalEngine();
		}
	}

	TSharedPtr<FAngelscriptType> ResolveHostTypeFromFirstParameter(UFunction* Function)
	{
		for (TFieldIterator<FProperty> It(Function); It && (It->PropertyFlags & CPF_Parm); ++It)
		{
			FProperty* Property = *It;
			if (Property->HasAnyPropertyFlags(CPF_ReturnParm))
			{
				continue;
			}

			if (const FStructProperty* StructProperty = CastField<FStructProperty>(Property))
			{
				return FAngelscriptTypeUsage::FromStruct(StructProperty->Struct).Type;
			}

			return FAngelscriptTypeUsage::FromProperty(Property).Type;
		}

		return nullptr;
	}
}

TEST_CLASS_WITH_FLAGS(FAngelscriptGameplayTagFunctionLibrarySignatureTest,
	"Angelscript.GameplayTags.FunctionLibraries.Signatures",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
{
	TEST_METHOD(GameplayTagQueryMixinSignature)
	{
		using namespace AngelscriptGameplayTags_FunctionLibrarySignatureTests_Private;

		ResetIsolatedEnvironment();
		ON_SCOPE_EXIT
		{
			ResetIsolatedEnvironment();
		};

		const FAngelscriptEngineDependencies Dependencies = FAngelscriptEngineDependencies::CreateDefault();
		TUniquePtr<FAngelscriptEngine> Engine = CreateScriptScanFreeFullEngineForTesting(FAngelscriptEngineConfig(), Dependencies);
		if (!TestRunner->TestTrue(TEXT("GameplayTagQueryMixinSignature should create a testing engine"), Engine.IsValid()))
		{
			return;
		}

		FAngelscriptEngineScope EngineScope(*Engine);

		UFunction* Function = UGameplayTagQueryMixinLibrary::StaticClass()->FindFunctionByName(TEXT("Matches"));
		if (!TestRunner->TestNotNull(TEXT("GameplayTagQueryMixinSignature should find Matches"), Function))
		{
			return;
		}

		TSharedPtr<FAngelscriptType> HostType = ResolveHostTypeFromFirstParameter(Function);
		if (!TestRunner->TestTrue(TEXT("GameplayTagQueryMixinSignature should resolve FGameplayTagQuery host type"), HostType.IsValid()))
		{
			return;
		}

		FAngelscriptFunctionSignature Signature(HostType.ToSharedRef(), Function, TEXT("Matches_GameplayTagQueryMixinInspection"));
		const int32 FunctionId = FAngelscriptBinds::BindMethodDirect(
			Signature.ClassName,
			Signature.Declaration,
			asFUNCTION(NoOpGeneric),
			asCALL_GENERIC,
			ASAutoCaller::FunctionCaller::Make());
		Signature.ModifyScriptFunction(FunctionId);

		auto* ScriptFunction = reinterpret_cast<asCScriptFunction*>(Engine->GetScriptEngine()->GetFunctionById(FunctionId));
		if (!TestRunner->TestNotNull(TEXT("GameplayTagQueryMixinSignature should create a script function"), ScriptFunction))
		{
			return;
		}

		const FString ScriptDeclaration = ANSI_TO_TCHAR(ScriptFunction->GetDeclaration(true, false, true, true));

		TestRunner->TestEqual(TEXT("GameplayTagQuery mixin should bind to FGameplayTagQuery"), HostType->GetAngelscriptTypeName(), FString(TEXT("FGameplayTagQuery")));
		TestRunner->TestTrue(TEXT("GameplayTagQuery mixin should remain static in Unreal"), Signature.bStaticInUnreal);
		TestRunner->TestFalse(TEXT("GameplayTagQuery mixin should bind as a script member"), Signature.bStaticInScript);
		TestRunner->TestEqual(TEXT("GameplayTagQuery mixin should expose the expected script owner"), Signature.ClassName, FString(TEXT("FGameplayTagQuery")));
		TestRunner->TestEqual(TEXT("GameplayTagQuery mixin should expose one public argument"), Signature.ArgumentTypes.Num(), 1);
		if (Signature.ArgumentTypes.Num() == 1)
		{
			TestRunner->TestTrue(
				TEXT("GameplayTagQuery mixin should expose FGameplayTagContainer argument"),
				Signature.ArgumentTypes[0].GetAngelscriptDeclaration().Contains(TEXT("FGameplayTagContainer")));
			TestRunner->TestTrue(
				TEXT("GameplayTagQuery mixin should preserve Tags argument name"),
				Signature.ArgumentNames[0] == TEXT("Tags"));
		}
		TestRunner->TestEqual(TEXT("GameplayTagQuery mixin should not hide a world-context argument"), static_cast<int32>(Signature.WorldContextArgument), -1);
		TestRunner->TestEqual(TEXT("GameplayTagQuery mixin script function should expose one argument"), static_cast<int32>(ScriptFunction->GetParamCount()), 1);
		TestRunner->TestEqual(TEXT("GameplayTagQuery mixin script function should not hide arguments"), static_cast<int32>(ScriptFunction->hiddenArgumentIndex), -1);
		TestRunner->TestTrue(TEXT("GameplayTagQuery mixin declaration should keep Matches"), Signature.Declaration.Contains(TEXT("Matches")));
		TestRunner->TestTrue(TEXT("GameplayTagQuery mixin script declaration should keep Matches"), ScriptDeclaration.Contains(TEXT("Matches")));
		TestRunner->TestTrue(TEXT("GameplayTagQuery mixin should generate a const declaration"), Signature.Declaration.Contains(TEXT("const")));
		TestRunner->TestTrue(TEXT("GameplayTagQuery mixin script declaration should stay const"), ScriptDeclaration.Contains(TEXT("const")));
	}
};

#endif
