// Fill out your copyright notice in the Description page of Project Settings.


#include "Hexagon.h"

Hexagon::Hexagon(FVector pcenter, int pindex, int pvertical, int phorizontal)
{
	_center = pcenter;
	_index = pindex;
	_vertical = pvertical;
	_horizontal = phorizontal;
}

Hexagon::~Hexagon()
{

}
