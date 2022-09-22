// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class HEXAGONGENERATORONLY_API Hexagon
{
public:
	Hexagon(FVector pcenter, int pindex, int pvertical, int phorizontal);
	~Hexagon();
	int _distance = -1;
	int _heapIndex = -1;

	bool GetLayerState(int layer) {
		return _layerState[layer];
	}
	void SetLayerState(int layer, bool state) {
		_layerState[layer] = state;
	}

	int GetIndex() {
		return _index;
	}
	int GetHorizontal() {
		return _horizontal;
	}
	int GetVertical() {
		return _vertical;
	}

	FVector GetCenter() {
		return _center;
	}

	void SetBlockState(bool blocked) {
		_blocked = blocked;
	}
	bool GetBlockStatus() {
		return _blocked;
	}

private:
	int _index = -1;
	bool _blocked = false;
	int _vertical = -1;
	int _horizontal = -1;

	FVector _center;
	TArray<bool> _edgeState;
	TArray<bool> _layerState = { false, false, false, false, false, false };

};
