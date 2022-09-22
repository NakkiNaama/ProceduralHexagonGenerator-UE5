// Niklas Grönholm
// Generates hexagon without duplicate verticies (Mostly for fun)
// This was taken from larger project so there might be still be some remnants from that

#include "WorldGenerator.h"
#include "Engine/World.h"



// Sets default values
AWorldGenerator::AWorldGenerator()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	USceneComponent* scene = CreateDefaultSubobject<USceneComponent>("RootScene");
	SetRootComponent(scene);

	_terragonMesh = CreateDefaultSubobject<UProceduralMeshComponent>("Terragon Mesh");
	_terragonMesh->bUseAsyncCooking = true;
	
}

// Called when the game starts or when spawned
void AWorldGenerator::BeginPlay()
{
	Super::BeginPlay();
	if (_mapSize % 2 == 0) {
		_mapSize--;
	}
	GenerateProceduralHexagon();
}


bool AWorldGenerator::GenerateProceduralHexagon() {

	DefineVerticies();
	FirstRow();

	for (uint32_t vertical = 1; vertical < uint32_t(_mapSize); vertical++) {
		for (uint32_t horizontal = 1; horizontal <= _rowSize[vertical]; horizontal++) {
			SetLastLines(_lastLineLeft, _lastLineRight, vertical, horizontal);
			if (vertical > uint32_t(_mapSize) / 2) {
				NextLineAfterHalf(vertical, horizontal);
			}
			else {
				if (horizontal != 1) {
					_lastCenter = _currentCenter;
					// First hexagon has more vertices than others
					if (horizontal == 2) _currentCenter += 2;
					_currentCenter += 3;
					if (horizontal != _rowSize[vertical]) {
						NextLine(vertical, horizontal);
					}
					else {
						LastHexagonOfLine(vertical, horizontal);
					}
				}
				else {
					FirstHexagonOfLine(vertical, horizontal);
				}
			}
		}
	}
	
	/*
	UKismetProceduralMeshLibrary::CalculateTangentsForMesh(_verticies, _triangles,
		_textureUV, _normals, _tangents);
	*/

	_terragonMesh->CreateMeshSection_LinearColor
	(
		0, // section index
		_verticies, // Verticies
		_triangles, // Triangles
		_normals,
		_textureUV, // UV
		TArray<FLinearColor>(),
		_tangents,
		true
	);
	return true;
}


inline void AWorldGenerator::DefineVerticies() {
	int horizontalSize = _mapSize / 2;
	int afterRow = 0;
	for (int vertical = 0; vertical < _mapSize; vertical++) {
		_rowIndexes.Add(_hexagons.Num());
		TArray<int> currentRow;
		bool afterHalf = vertical > _mapSize / 2;
		if (afterHalf) {
			horizontalSize--;
			afterRow++;
		}
		else horizontalSize++;
		for (int horizontal = 0; horizontal < horizontalSize; horizontal++) {
			float hori;
			if (afterHalf) {
				hori = (_scale * float(horizontal) * 2);
				hori -= (_mapSize - vertical) * _scale;
				hori += _scale;
			}
			else { // (_scale * vertical); <-- relevant part?
				hori = (_scale * float(horizontal) * 2) - (_scale * vertical);
			}
			FVector offset = { -_scale * float(vertical) * 1.5f, hori, 0 };
			
		
			AddNewHexagon(FVector(0, 0, 0) + offset);
			_hexagonCenterLocations.Add(_verticies.Last());
			int horiOffset = 0;
			if (vertical > _mapSize / 2) {
				horiOffset += vertical - _mapSize / 2;
			}
			//_hexagons.Add(Hexagon());
			_hexagons.Add(Hexagon(_verticies.Last(), _hexagons.Num(), vertical, horizontal + horiOffset));
			//UE_LOG(LogTemp, Warning, TEXT("hexagon spawn %d"), _hexagons.Num());
			_centers.Add(_verticies.Num() - 1);
			currentRow.Add(_verticies.Num() - 1);

			// These verticies are only needed for the first row
			if (vertical == 0) {
				AddNewHexagon(FVector(_scale, 0, 0) + offset); // North
				AddNewHexagon(FVector(_scale / 2, _scale, 0) + offset); // NorthEast
			}
			else if (horizontal == horizontalSize - 1 && vertical <= _mapSize / 2) { // last of row vertex add
				AddNewHexagon(FVector(_scale / 2, _scale, 0) + offset); // NorthEast
			}
			AddNewHexagon(FVector(-_scale / 2, _scale, 0) + offset); // SouthEast
			AddNewHexagon(FVector(-_scale, 0, 0) + offset); // South

			// Only first hexagon in row needs these verticies
			if (horizontal == 0) {
				AddNewHexagon(FVector(-_scale / 2, -_scale, 0) + offset); // SouthWest

				if (vertical <= _mapSize / 2) {
					AddNewHexagon(FVector(_scale / 2, -_scale, 0) + offset); // NorthWest
				}
			}		
		}
		_rowCenters.Add(currentRow);
		_rowSize.Add(horizontalSize);
	}
}



inline void AWorldGenerator::SetLastLines(int& left, int& right, uint32_t vertical, uint32_t horizontal) {
	if (vertical > uint32_t(_mapSize) / 2) {
		right = _rowCenters[vertical - 1][horizontal];
		if (horizontal == _rowCenters[vertical].Num() && vertical - 1 == uint32_t(_mapSize) / 2) {
			right++;
		}
		if (horizontal >= 1) {
			left = _rowCenters[vertical - 1][horizontal - 1];
		}
		else left = -1;
	}
	else {
		if (vertical == 1) {
			right = 5 * (horizontal - 1) + 2;
			left = right - 5;
			if (horizontal == 2) left -= 2;
		}
		else {
			if (uint32_t(_rowCenters[vertical - 1].Num()) > horizontal - 1
				&& horizontal < _rowSize[vertical]) {
				right = _rowCenters[vertical - 1][horizontal - 1];
				if (horizontal == _rowCenters[vertical - 1].Num()) {
					right++;
				}
			}
			else right = -1;
			if (horizontal >= 2) {
				left = _rowCenters[vertical - 1][horizontal - 2];
			}
			else left = -1;
		}
	}
}


inline void AWorldGenerator::FirstHexagonOfLine(int vertical, int horizontal) {
	// first hexagon of row needs unique verticies
	if (vertical == 1) {
		// North East
		_triangles.Add(4);
		_triangles.Add(5);
		_triangles.Add(_currentCenter);
		// East
		_triangles.Add(_currentCenter);
		_triangles.Add(_currentCenter + 1);
		_triangles.Add(4);
		// South East
		_triangles.Add(_currentCenter + 2);
		_triangles.Add(_currentCenter + 1);
		_triangles.Add(_currentCenter);
		// South West		
		_triangles.Add(_currentCenter + 3);
		_triangles.Add(_currentCenter + 2);
		_triangles.Add(_currentCenter);
		// West
		_triangles.Add(_currentCenter + 4);
		_triangles.Add(_currentCenter + 3);
		_triangles.Add(_currentCenter);
		// NorthWest
		_triangles.Add(_currentCenter + 4);
		_triangles.Add(_currentCenter);
		_triangles.Add(5);
	}
	else {
		// First hexagon of line for later than first row
		_currentCenter += 4;
		// North East				
		_triangles.Add(_lastLineRight + 2);
		_triangles.Add(_lastLineRight + 3);
		_triangles.Add(_currentCenter);
		// East
		_triangles.Add(_currentCenter + 1);
		_triangles.Add(_lastLineRight + 2);
		_triangles.Add(_currentCenter);
		// South East
		_triangles.Add(_currentCenter + 2);
		_triangles.Add(_currentCenter + 1);
		_triangles.Add(_currentCenter);
		// South West		
		_triangles.Add(_currentCenter + 3);
		_triangles.Add(_currentCenter + 2);
		_triangles.Add(_currentCenter);
		// West
		_triangles.Add(_currentCenter + 4);
		_triangles.Add(_currentCenter + 3);
		_triangles.Add(_currentCenter);
		// NorthWest
		_triangles.Add(_currentCenter + 4);
		_triangles.Add(_currentCenter);
		_triangles.Add(_lastLineRight + 3);
	}
}

inline void AWorldGenerator::LastHexagonOfLine(int vertical, int horizontal) {
	if (vertical == 1) {
		// Last Hexagon for first line
		// North East
		_triangles.Add(_currentCenter + 1);
		_triangles.Add(_lastLineLeft + 3);
		_triangles.Add(_currentCenter);
		// East
		_triangles.Add(_currentCenter + 2);
		_triangles.Add(_currentCenter + 1);
		_triangles.Add(_currentCenter);
		// South East
		_triangles.Add(_currentCenter + 3);
		_triangles.Add(_currentCenter + 2);
		_triangles.Add(_currentCenter);
		// South West		
		_triangles.Add(_lastCenter + 1);
		_triangles.Add(_currentCenter + 3);
		_triangles.Add(_currentCenter);
		// West
		_triangles.Add(_currentCenter);
		_triangles.Add(_lastLineLeft + 4);
		_triangles.Add(_lastCenter + 1);
		// NorthWest
		_triangles.Add(_currentCenter);
		_triangles.Add(_lastLineLeft + 3);
		_triangles.Add(_lastLineLeft + 4);
	}
	else {
		// Last hexagon for later rows
		// North East
		_triangles.Add(_currentCenter + 1);
		_triangles.Add(_lastLineLeft + 2);
		_triangles.Add(_currentCenter);
		// East
		_triangles.Add(_currentCenter + 2);
		_triangles.Add(_currentCenter + 1);
		_triangles.Add(_currentCenter);
		// South East
		_triangles.Add(_currentCenter + 3);
		_triangles.Add(_currentCenter + 2);
		_triangles.Add(_currentCenter);
		// South West		
		_triangles.Add(_lastCenter + 1);
		_triangles.Add(_currentCenter + 3);
		_triangles.Add(_currentCenter);
		// West
		_triangles.Add(_lastLineLeft + 3);
		_triangles.Add(_lastCenter + 1);
		_triangles.Add(_currentCenter);
		// NorthWest
		_triangles.Add(_currentCenter);
		_triangles.Add(_lastLineLeft + 2);
		_triangles.Add(_lastLineLeft + 3);
	}
}

inline void AWorldGenerator::FirstRow() {
	// First hexagon *******************************************
// North East
	_triangles.Add(2); _triangles.Add(1); _triangles.Add(0);
	// East
	_triangles.Add(3); _triangles.Add(2); _triangles.Add(0);
	// South East
	_triangles.Add(4); _triangles.Add(3); _triangles.Add(0);
	// South West
	_triangles.Add(5); _triangles.Add(4); _triangles.Add(0);
	// West
	_triangles.Add(6); _triangles.Add(5); _triangles.Add(0);
	// NorthWest
	_triangles.Add(1); _triangles.Add(6); _triangles.Add(0);
	// **********************************************************

	// Add Triangles
	int skipCount = 2; // Only for first hexagon
	// Generates only the first row of hexagons
	for (uint32_t i = 1; i < _rowSize[0]; i++) {
		_currentCenter = i * 7 - (i - 1) * 2;
		// North East
		_triangles.Add(2 + _currentCenter);
		_triangles.Add(1 + _currentCenter);
		_triangles.Add(0 + _currentCenter);
		// East
		_triangles.Add(3 + _currentCenter);
		_triangles.Add(2 + _currentCenter);
		_triangles.Add(0 + _currentCenter);
		// South East
		_triangles.Add(4 + _currentCenter);
		_triangles.Add(3 + _currentCenter);
		_triangles.Add(0 + _currentCenter);
		// South West		
		_triangles.Add(4 + _currentCenter);
		_triangles.Add(0 + _currentCenter);
		_triangles.Add(5 + _currentCenter - (7 + skipCount));
		// West
		_triangles.Add(6 + _currentCenter - (9 + skipCount));
		_triangles.Add(5 + _currentCenter - (7 + skipCount));
		_triangles.Add(0 + _currentCenter);
		// NorthWest
		_triangles.Add(1 + _currentCenter);
		_triangles.Add(6 + _currentCenter - (9 + skipCount));
		_triangles.Add(0 + _currentCenter);

		skipCount = 0; // Resets skip count because after first iteration there is no need for it.
	}
	_currentCenter += 5;
}


inline void AWorldGenerator::NextLineAfterHalf(uint32_t vertical, uint32_t horizontal) {
	_lastCenter = _currentCenter;
	// First hexagon has more vertices than others
	if (horizontal < 3) _currentCenter += 1;
	if (vertical > uint32_t(_mapSize) / 2 + 1 && horizontal == 1) _currentCenter--;
	_currentCenter += 3;
	// North East
	_triangles.Add(_lastLineRight + 2);
	_triangles.Add(_lastLineLeft + 1);
	_triangles.Add(_currentCenter);
	// East
	_triangles.Add(_currentCenter + 1);
	_triangles.Add(_lastLineRight + 2);
	_triangles.Add(_currentCenter);
	// South East
	_triangles.Add(_currentCenter + 2);
	_triangles.Add(_currentCenter + 1);
	_triangles.Add(_currentCenter);

	if (horizontal == 1) {
		// South West		
		_triangles.Add(_currentCenter + 3);
		_triangles.Add(_currentCenter + 2);
		_triangles.Add(_currentCenter);
	}
	else {
		// South West		
		_triangles.Add(_lastCenter + 1);
		_triangles.Add(_currentCenter + 2);
		_triangles.Add(_currentCenter);
	}
	if (horizontal == 1) {
		// West
		_triangles.Add(_lastLineLeft + 2);
		_triangles.Add(_currentCenter + 3);
		_triangles.Add(_currentCenter);
	}
	else {
		// West
		_triangles.Add(_lastLineLeft + 2);
		_triangles.Add(_lastCenter + 1);
		_triangles.Add(_currentCenter);
	}
	// NorthWest
	_triangles.Add(_currentCenter);
	_triangles.Add(_lastLineLeft + 1);
	_triangles.Add(_lastLineLeft + 2);
}


inline void AWorldGenerator::NextLine(int vertical, int horizontal) {
	if (vertical == 1) {
		// North East
		_triangles.Add(_lastLineRight + 4);
		_triangles.Add(_lastLineLeft + 3);
		_triangles.Add(_currentCenter);
		// East
		_triangles.Add(_currentCenter);
		_triangles.Add(_currentCenter + 1);
		_triangles.Add(_lastLineRight + 4);
		// South East
		_triangles.Add(_currentCenter + 2);
		_triangles.Add(_currentCenter + 1);
		_triangles.Add(_currentCenter);
		// South West		
		_triangles.Add(_lastCenter + 1);
		_triangles.Add(_currentCenter + 2);
		_triangles.Add(_currentCenter);
		// West
		_triangles.Add(_currentCenter);
		_triangles.Add(_lastLineLeft + 4);
		_triangles.Add(_lastCenter + 1);
		// NorthWest
		_triangles.Add(_currentCenter);
		_triangles.Add(_lastLineLeft + 3);
		_triangles.Add(_lastLineLeft + 4);
	}
	else {
		// third row
		// North East
		_triangles.Add(_lastLineRight + 2);
		_triangles.Add(_lastLineLeft + 1);
		_triangles.Add(_currentCenter);
		// East
		_triangles.Add(_currentCenter + 1);
		_triangles.Add(_lastLineRight + 2);
		_triangles.Add(_currentCenter);
		// South East
		_triangles.Add(_currentCenter + 2);
		_triangles.Add(_currentCenter + 1);
		_triangles.Add(_currentCenter);
		// South West		
		_triangles.Add(_lastCenter + 1);
		_triangles.Add(_currentCenter + 2);
		_triangles.Add(_currentCenter);
		// West
		_triangles.Add(_currentCenter);
		_triangles.Add(_lastLineLeft + 2);
		_triangles.Add(_lastCenter + 1);
		// NorthWest
		_triangles.Add(_currentCenter);
		_triangles.Add(_lastLineLeft + 1);
		_triangles.Add(_lastLineLeft + 2);
	}
}


// Called every frame
void AWorldGenerator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


 void AWorldGenerator::AddNewHexagon(FVector vertex) {
	FVector vertexPos = vertex;
	float multiplier = 1.f;
	float freqMultiplier = 1.f;
	
	for (int i = 0; i < _layerCount; i++) {
		FVector2D xy = FVector2D(vertexPos) + FVector2D(1.f / freqMultiplier);
		xy *= FVector2D(_heightFreq * freqMultiplier);
		
		//vertexPos.Z += (PerlinNoise(xy * FVector2D(_heightFreq * freqMultiplier), _seed ^ i) + 1.f) * _heightVariation * multiplier;
		multiplier *= _heightExponent;
		freqMultiplier *= 2.f;
	}
	
	_verticies.Add(vertexPos);
	_textureUV.Add(FVector2D(vertexPos) * _texScale + _texOffset);
}


