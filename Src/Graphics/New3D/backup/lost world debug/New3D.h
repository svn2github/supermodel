/**
** Supermodel
** A Sega Model 3 Arcade Emulator.
** Copyright 2011 Bart Trzynadlowski, Nik Henson
**
** This file is part of Supermodel.
**
** Supermodel is free software: you can redistribute it and/or modify it under
** the terms of the GNU General Public License as published by the Free
** Software Foundation, either version 3 of the License, or (at your option)
** any later version.
**
** Supermodel is distributed in the hope that it will be useful, but WITHOUT
** ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
** FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
** more details.
**
** You should have received a copy of the GNU General Public License along
** with Supermodel.  If not, see <http://www.gnu.org/licenses/>.
**/

/*
* New3D.h
*
* Header file defining the CNew3D class: OpenGL Real3D graphics engine.
*/

#ifndef INCLUDED_NEW3D_H
#define INCLUDED_NEW3D_H

#include "Pkgs/glew.h"
#include "Types.h"
#include "TextureSheet.h"
#include "Graphics/IRender3D.h"
#include "Model.h"
#include "Mat4.h"
#include "R3DShader.h"
#include "VBO.h"
#include "R3DData.h"
#include "Plane.h"
#include "Vec.h"
#include "R3DScrollFog.h"

namespace New3D {

class CNew3D : public IRender3D
{
public:
	/*
	* RenderFrame(void):
	*
	* Renders the complete scene database. Must be called between BeginFrame() and
	* EndFrame(). This function traverses the scene database and builds up display
	* lists.
	*/
	void RenderFrame(void);

	/*
	* BeginFrame(void):
	*
	* Prepare to render a new frame. Must be called once per frame prior to
	* drawing anything.
	*/
	void BeginFrame(void);

	/*
	* EndFrame(void):
	*
	* Signals the end of rendering for this frame. Must be called last during
	* the frame.
	*/
	void EndFrame(void);

	/*
	* UploadTextures(x, y, width, height):
	*
	* Signals that a portion of texture RAM has been updated.
	*
	* Parameters:
	*		x		X position within texture RAM.
	*		y		Y position within texture RAM.
	*		width	Width of texture data in texels.
	*		height	Height.
	*/
	void UploadTextures(unsigned x, unsigned y, unsigned width, unsigned height);

	/*
	* AttachMemory(cullingRAMLoPtr, cullingRAMHiPtr, polyRAMPtr, vromPtr,
	* 				textureRAMPtr):
	*
	* Attaches RAM and ROM areas. This must be done prior to any rendering
	* otherwise the program may crash with an access violation.
	*
	* Parameters:
	*		cullingRAMLoPtr		Pointer to low culling RAM (4 MB).
	*		cullingRAMHiPtr		Pointer to high culling RAM (1 MB).
	*		polyRAMPtr			Pointer to polygon RAM (4 MB).
	*		vromPtr				Pointer to video ROM (64 MB).
	*		textureRAMPtr		Pointer to texture RAM (8 MB).
	*/
	void AttachMemory(const UINT32 *cullingRAMLoPtr,
		const UINT32 *cullingRAMHiPtr, const UINT32 *polyRAMPtr,
		const UINT32 *vromPtr, const UINT16 *textureRAMPtr);

	/*
	* SetStep(stepID):
	*
	* Sets the Model 3 hardware stepping, which also determines the Real3D
	* functionality. The default is Step 1.0. This should be called prior to
	* any other emulation functions and after Init().
	*
	* Parameters:
	*		stepID	0x10 for Step 1.0, 0x15 for Step 1.5, 0x20 for Step 2.0,
	*				or 0x21 for Step 2.1. Anything else defaults to 1.0.
	*/
	void SetStep(int stepID);

	/*
	* Init(xOffset, yOffset, xRes, yRes, totalXRes, totalYRes):
	*
	* One-time initialization of the context. Must be called before any other
	* members (meaning it should be called even before being attached to any
	* other objects that want to use it).
	*
	* External shader files are loaded according to configuration settings.
	*
	* Parameters:
	*		xOffset		X offset of the viewable area within OpenGL display
	*                  surface, in pixels.
	*		yOffset		Y offset.
	*		xRes		Horizontal resolution of the viewable area.
	*		yRes		Vertical resolution.
	*		totalXRes	Horizontal resolution of the complete display area.
	*		totalYRes	Vertical resolution.
	*
	* Returns:
	*		OKAY is successful, otherwise FAILED if a non-recoverable error
	*		occurred. Any allocated memory will not be freed until the
	*		destructor is called. Prints own error messages.
	*/
	bool Init(unsigned xOffset, unsigned yOffset, unsigned xRes, unsigned yRes, unsigned totalXRes, unsigned totalYRes);

	/*
	* CRender3D(void):
	* ~CRender3D(void):
	*
	* Constructor and destructor.
	*/
	CNew3D(void);
	~CNew3D(void);

private:
	/*
	* Private Members
	*/

	// Real3D address translation
	const UINT32 *TranslateCullingAddress(UINT32 addr);
	const UINT32 *TranslateModelAddress(UINT32 addr);

	// Matrix stack
	void MultMatrix(UINT32 matrixOffset, Mat4& mat);
	void InitMatrixStack(UINT32 matrixBaseAddr, Mat4& mat);

	// Scene database traversal
	bool DrawModel(UINT32 modelAddr);
	void DescendCullingNode(UINT32 addr);
	void DescendPointerList(UINT32 addr);
	void DescendNodePtr(UINT32 nodeAddr);
	void RenderViewport(UINT32 addr);

	// building the scene
	void CacheModel(Model *m, const UINT32 *data);
	void CopyVertexData(const R3DPoly& r3dPoly, std::vector<Poly>& polyArray);
	void OffsetTexCoords(R3DPoly& r3dPoly, float offset[2]);

	void RenderScene(int priority, bool alpha);
	float Determinant3x3(const float m[16]);
	bool IsDynamicModel(UINT32 *data);		// check if the model has a colour palette
	bool IsVROMModel(UINT32 modelAddr);
	void DrawScrollFog();

	void CalcTexOffset(int offX, int offY, int page, int x, int y, int& newX, int& newY);	

	/*
	* Data
	*/

	// Stepping
	int		m_step;
	int		m_offset;			// offset to subtract for words 3 and higher of culling nodes
	float	m_vertexFactor;		// fixed-point conversion factor for vertices

	// Memory (passed from outside)
	const UINT32	*m_cullingRAMLo;	// 4 MB
	const UINT32	*m_cullingRAMHi;	// 1 MB
	const UINT32	*m_polyRAM;			// 4 MB
	const UINT32	*m_vrom;			// 64 MB
	const UINT16	*m_textureRAM;		// 8 MB

	// Resolution and scaling factors (to support resolutions higher than 496x384) and offsets
	float		m_xRatio, m_yRatio;
	unsigned	m_xOffs, m_yOffs;
	unsigned 	m_totalXRes, m_totalYRes;

	// Real3D Base Matrix Pointer
	const float	*m_matrixBasePtr;
	UINT32 m_colorTableAddr = 0x400;		// address of color table in polygon RAM

	TextureSheet	m_texSheet;
	NodeAttributes	m_nodeAttribs;
	Mat4			m_modelMat;				// current modelview matrix

	std::vector<Node> m_nodes;				// this represents the entire render frame
	std::vector<Poly> m_polyBufferRam;		// dynamic polys
	std::vector<Poly> m_polyBufferRom;		// rom polys
	std::unordered_map<UINT32, std::shared_ptr<std::vector<Mesh>>> m_romMap;	// a hash table for all the ROM models. The meshes don't have model matrices or tex offsets yet

	VBO m_vbo;								// large VBO to hold our poly data, start of VBO is ROM data, ram polys follow
	R3DShader m_r3dShader;
	R3DScrollFog m_r3dScrollFog;

	Plane m_planes[6];

	struct BBox
	{
		V4::Vec4 points[8];
	};

	void CalcFrustumPlanes	(Plane p[6], const float* matrix);
	void CalcBox			(float distance, BBox& box);
	void TransformBox		(const float *m, BBox& box);
	void MultVec			(const float matrix[16], const float in[4], float out[4]);
	Clip ClipBox			(BBox& box, Plane planes[6]);

	void TestDraw();
};

} // New3D

#endif  // INCLUDED_NEW3D_H