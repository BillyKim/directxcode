#ifndef __CUBE_H__
#define __CUBE_H__

#include "d3dx9.h"

struct Vertex
{
	float  x,  y,  z;	// position
	float  nx, ny, nz;	// normal
	float  u,  v;		// texture
};

class Cube
{
public:
	Cube(void);
	~Cube(void);

	void Init(D3DXVECTOR3& top_left_front_point);
	void SetDevice(LPDIRECT3DDEVICE9 pDevice);
	void SetTextureId(int faceId, int textureId);
	void UpdateMinMaxPoints(D3DXVECTOR3& rotate_axis, int num_half_PI);
	void UpdateCenter();
	void UpdateLayerId();
	void Rotate(D3DXVECTOR3& axis, float angle);
	void Draw(ID3DXEffect* effects, D3DXMATRIX& view_matrix, D3DXMATRIX& proj_matrix, D3DXVECTOR3& eye_pos);

	float GetLength() const;

	D3DXVECTOR3 GetMinPoint() const;
	D3DXVECTOR3 GetMaxPoint() const;
	D3DXVECTOR3 GetCenter() const;

	void SetWorldMatrix(D3DXMATRIX& world_matrix);

	// Set layer id
	void SetLayerIdX(int layer_id_x);
	void SetLayerIdY(int layer_id_y);
	void SetLayerIdZ(int layer_id_z);

	// Determine whether cube in a given layer
	bool InLayer(int layer_id);

private:
	void InitBuffers(D3DXVECTOR3& front_bottom_left);
	void InitVertexBuffer(D3DXVECTOR3& front_bottom_left);
	void InitIndexBuffer();
	void InitCornerPoints(D3DXVECTOR3& front_bottom_left_point);	// Initialize corner points.
	D3DXVECTOR3 CalculateCenter(D3DXVECTOR3& min_point, D3DXVECTOR3& max_point);
	void InitLayerIds();

private:
	float length_;								// side length_ of the cube.
	D3DXVECTOR3 max_point_;						// The max corner point of the cube(back-top-right corner)
	D3DXVECTOR3 min_point_;						// The min corner point of the cube(front-bottom-left corner)
	D3DXVECTOR3 center_;						// Cube center
	static const int kNumFaces_ = 6;			// The number of faces in a cube, this is always 6.
	const int kNumCornerPoints_;				// Number of corner points of the cube
	int textureId[kNumFaces_];					// the index is the faceId, the value is the textureId.

	// Each cube in the Rubik Cube has 3 layer id, for a 3 x 3 Rubik Cube, the layer id was count as below:
	// along X axis, from negative to positive(left -> right) 0, 1, 2
	// along Y axis, from negative to positive(bottom -> top) 3, 4, 5
	// along Z axis, from negative to positive(front -> back) 6, 7, 8
	// So the front-top-left corner cube's id is:
	// layer_x_ = 0, layer_y_ = 5, layer_z_ = 6
	int layer_id_x_;
	int layer_id_y_;
	int layer_id_z_;

	LPDIRECT3DINDEXBUFFER9  pIB[kNumFaces_] ;
	D3DXVECTOR3*			corner_points_;		// array to store the 8 corner poinst of the cube 
	LPDIRECT3DVERTEXBUFFER9 vertex_buffer_ ;
	LPDIRECT3DDEVICE9		d3d_device_ ;
	IDirect3DVertexDeclaration9* vertex_declare_;
	D3DXMATRIX				world_matrix_ ;		// world matrix for unit cube, for rotation.

	D3DXHANDLE				technique_;
	D3DXHANDLE				handle_faceid;
	D3DXHANDLE				handle_wvp_matrix_;
	D3DXHANDLE				handle_eye_position_;
};

#endif // end __CUBE_H__