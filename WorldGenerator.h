// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Engine/StaticMeshActor.h"
#include "KismetProceduralMeshLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Hexagon.h"
#include "WorldGenerator.generated.h"


UCLASS()
class HEXAGONGENERATORONLY_API AWorldGenerator : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWorldGenerator();

	void AddNewHexagon(FVector vertex);

	int GetHexagonDistance(int index) {
		return _hexagons[index]._distance;
	}



	bool GenerateProceduralHexagon();


	const float _scale = 35;
	UFUNCTION(BlueprintPure)
	TArray<FVector> GetCenters() {
		return _hexagonCenterLocations;
	}
	UFUNCTION(BlueprintPure)
	TArray<FVector> GetNormals() {
		return _normals;
	}
	UFUNCTION(BlueprintPure)
	int GetCenterIndex(int i) {
		return _centers[i];
	}

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		UProceduralMeshComponent* _terragonMesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		UProceduralMeshComponent* _bonusgonMesh;
	UPROPERTY(EditAnywhere)
		int _mapSize = 99;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grass Types", meta = (AllowPrivateAccess = "true"))
		UStaticMesh* _grass;

	UPROPERTY(EditAnywhere)
		FVector2D _texScale = FVector2D(1.f / 1000, 1.f / 1000.f);
	UPROPERTY(EditAnywhere)
		FVector2D _texOffset = FVector2D(0, 0);

	UPROPERTY(EditAnywhere)
		float _heightVariation = 100;
	UPROPERTY(EditAnywhere)
		float _heightFreq = 0.01;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int _seed = 1;
	UPROPERTY(EditAnywhere)
		int _layerCount = 2;
	UPROPERTY(EditAnywhere)
		float _heightExponent = 0.5f;

	FVector GetHexagonCenter(int index) {
		return _hexagons[index].GetCenter();
	}




private:
	void DefineVerticies();
	void SetLastLines(int& left, int& right, uint32_t vertical, uint32_t horizontal);

	void FirstHexagonOfLine(int vertical, int horizontal);
	void LastHexagonOfLine(int vertical, int horizontal);
	void NextLine(int vertical, int horizontal);

	void FirstRow();
	void NextLineAfterHalf(uint32_t vertical, uint32_t horizontal);
	

	
	int _currentCenter = 0; // Center of current hexagon
	int _lastLineRight = 0; // prev line - right hexagon
	int _lastLineLeft = 0; // prev line - left hexagon
	int _lastCenter = 0; // previous hexagon
	
	//TArray<FVector> _centers;
	TArray<FVector> _verticies;
	TArray<int> _triangles;
	TArray<uint32_t> _rowSize; // lenghts of rows
	TArray<TArray<int>> _rowCenters; // contains index of center verticies
	TArray<FVector2D> _textureUV;

	TArray<FVector> _normals;
	TArray<FProcMeshTangent> _tangents;
	TArray<int> _centers;
	TArray<int> _rowIndexes;
	
	// Bonusgons

	TArray<FVector> _hexagonCenterLocations; // should be removed
	TArray<Hexagon> _hexagons;


	TArray<Hexagon*> _bonusgonHeap;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
