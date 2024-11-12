// Copyright 2020-2023: Jesse J. van Vliet


#include "LinkerComponent.h"

#include "FarmionRem/Io/Linked.h"

#if WITH_EDITORONLY_DATA
#include "FarmionRem/Util/Debug/DebugEditorLineCompToMultiActors.h"
#endif

#include "Net/UnrealNetwork.h"

ULinkerComponent::ULinkerComponent() {
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
}

void ULinkerComponent::OnRep_Links() {
	OnLink.Broadcast();
}

AActor* ULinkerComponent::GetLinkedActor(FName Name) const {
	const FLinkerData* FoundLink = Links.FindByPredicate([Name](const FLinkerData& Link) { return Link.Name == Name; });
	if (FoundLink) {
		return FoundLink->Actor;
	}
	return nullptr;
}

void ULinkerComponent::BeginPlay() {
	Super::BeginPlay();

	TScriptInterface<ILinked> LinkedOwner = TScriptInterface<ILinked>(GetOwner());
	check(LinkedOwner); // Implement ILinked into the actor that owns this component..
	
	LinkedOwner->BindEvent(this);

	if (Links.IsEmpty()) {
		return;
	}

	// If any is nullptr at this point, we're loading, and we don't want to broadcast.
	for (const FLinkerData& Link : Links) {
		if (Link.Name == NAME_None) {
			return;
		}
		
		if (Link.Actor == nullptr) {
			return;
		}
	}
	
	OnLink.Broadcast();
	
}

void ULinkerComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ULinkerComponent, Links);
}

#if WITH_EDITORONLY_DATA
void ULinkerComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) {
	Super::PostEditChangeProperty(PropertyChangedEvent);

	FName PropertyName = (PropertyChangedEvent.Property != nullptr) ? PropertyChangedEvent.Property->GetFName() : NAME_None;
	if (PropertyName == GET_MEMBER_NAME_CHECKED(ULinkerComponent, Links)) {
		DebugLineComponent = GetOwner()->FindComponentByClass<UDebugEditorLineCompToMultiActors>();

		if (DebugLineComponent) {
			if (Links.IsEmpty()) {
				GetOwner()->RemoveOwnedComponent(DebugLineComponent);
				return;
			}
		} else {
			DebugLineComponent = NewObject<UDebugEditorLineCompToMultiActors>(GetOwner());
			DebugLineComponent->RegisterComponent();
			DebugLineComponent->StartComponentName = TEXT("Transform");
		}

		TArray<AActor*> ActorsInLinks;
		for (const auto& [Name, Actor] : Links) {
			if (Actor) {
				ActorsInLinks.Add(Actor);
			}
		}
			
		DebugLineComponent->EndActors = ActorsInLinks;
	}
}
#endif
