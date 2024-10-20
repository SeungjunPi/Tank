#include "StaticData.h"

Vertex TANK_VERTICES[7] = 
{ 
							{ 0, -1.f, 0, '*' }, 
	{ -1.f, 0, 0, 'O' },	{ 0, 0, 0, '*' },		{ 1.f, 0, 0, 'O' },
	{ -1.f, 1.f, 0, 'O' },	{ 0, 1.f, 0, ' ' },		{ 1.f, 1.f, 0, 'O' },
};
Model g_pTankModel = { TANK_VERTICES, 7 };

Vertex PROJECTILE_VERTICES[1] =
{
	{ 0.f, 0.f, 0.f, '*' }
};
Model g_pProjectileModel = { PROJECTILE_VERTICES, 1 };

Vertex OBSTACLE_VERTICES[9] =
{
	{-1.f, -1.f, 0, '<'}, {0.f, -1.f, 0, '^'}, {1.f, -1.f, 0, '>'},
	{ -1.f, 0.f, 0, '<' }, {0.f, 0.f, 0, ' '}, {1.f, 0.f, 0, '>'},
	{-1.f, 1.f, 0, '<'}, {0.f, 1.f, 0, 'v'}, {1.f, 1.f, 0, '>'}
};
Model g_pObstacleModel = { OBSTACLE_VERTICES, 9 };