//
//  CSGraphics.h
//  CDK
//
//  Created by 김찬 on 12. 8. 3..
//  Copyright (c) 2012년 brgames. All rights reserved.
//

#ifndef __CDK__CSGraphics__
#define __CDK__CSGraphics__

#include "CSRenderTarget.h"
#include "CSColor.h"
#include "CSMatrix.h"
#include "CSBlendMode.h"
#include "CSMaskingMode.h"
#include "CSZRect.h"
#include "CSLight.h"
#include "CSMaterial.h"
#include "CSCamera.h"
#include "CSConvolution.h"
#include "CSRootImage.h"
#include "CSVertexArray.h"
#include "CSRenderBounds.h"
#include "CSLocaleString.h"
#include "CSStringDisplay.h"

#ifdef CDK_IMPL
#include "CSScreenBuffer.h"
#endif

#include "CSFont.h"

enum CSAlign : uint {
    CSAlignNone = 0,
    CSAlignCenter = 1,
    CSAlignRight = 2,
    CSAlignMiddle = 4,
    CSAlignBottom = 8,
    CSAlignCenterMiddle = CSAlignCenter | CSAlignMiddle,
    CSAlignCenterBottom = CSAlignCenter | CSAlignBottom,
    CSAlignRightMiddle = CSAlignRight | CSAlignMiddle,
    CSAlignRightBottom = CSAlignRight | CSAlignBottom
};

enum {
    CSCornerLeftTop = 1,
    CSCornerRightTop = 2,
    CSCornerLeftBottom = 4,
    CSCornerRightBottom = 8,
    CSCornerAll = 15
};
typedef uint CSCorner;

enum CSStrokeMode : byte {
    CSStrokeNormal,
    CSStrokeLighten
};

enum CSCullMode : byte {
    CSCullNone,
    CSCullBack,
    CSCullFront
};

enum CSDepthMode : byte {
    CSDepthNone,
    CSDepthRead,
    CSDepthWrite,
    CSDepthReadWrite
};

enum CSStencilMode : byte {
    CSStencilNone,
    CSStencilInclusive,
    CSStencilExclusive
};

enum CSGraphicsValidation {
    CSGraphicsValidationTarget = 1,
    CSGraphicsValidationBlend = 2,
    CSGraphicsValidationCull = 4,
    CSGraphicsValidationDepth = 8,
    CSGraphicsValidationStencil = 16,
    CSGraphicsValidationLineWidth = 32,
    CSGraphicsValidationScissor = 64,
    CSGraphicsValidationAll = 127
};

class CSRenderProgram;
class CSGraphics;

typedef CSDelegate<void, CSGraphics*, const byte*> CSSpriteDelegate;

typedef CSDelegate0<void, CSGraphics*, const byte*> CSSpriteDelegate0;
template <typename Param1>
using CSSpriteDelegate1 = CSDelegate1<void, Param1, CSGraphics*, const byte*>;
template <typename Param1, typename Param2>
using CSSpriteDelegate2 = CSDelegate2<void, Param1, Param2, CSGraphics*, const byte*>;
template <typename Param1, typename Param2, typename Param3>
using CSSpriteDelegate3 = CSDelegate3<void, Param1, Param2, Param3, CSGraphics*, const byte*>;

#define CSStringWidthUnlimited  100000

class CSGraphics : public CSObject {
private:
    struct Geometry {
        CSCamera camera;
        CSConvolution convolution;
        struct Var {
            float lightness;
            float brightness;
            float contrast;
            float saturation;
            CSBlendMode blendMode;
            CSMaskingMode maskingMode;
            CSCullMode cullMode;
            CSDepthMode depthMode;
            CSStencilMode stencilMode;
            byte stencilDepth;
            byte layer;
            bool usingAlphaTest;
            bool usingScissor;
            bool usingLight;
            bool usingShadow;
            float lineWidth;

            struct Sub {
                byte strokeWidth;
                CSStrokeMode strokeMode;
                CSColor strokeColor;
                float depthBias;
                float alphaTest;
                CSRect scissor;
                CSLight light;
                CSMaterial material;
            } sub;
        } var;
        
        Geometry();
        Geometry(const Geometry& other);
        
        Geometry& operator =(const Geometry& other);
        
        bool operator ==(const Geometry& other) const;
        inline bool operator !=(const Geometry& other) const {
            return !(*this == other);
        }
        bool usingStroke() const;
    };
    
    struct DynamicGeometrySource {
        CSArray<ushort>* indices;
		uint indexOffset;
        uint mode;
        
        DynamicGeometrySource(uint mode);
        ~DynamicGeometrySource();
    };
    struct StaticGeometrySource {
        const CSVertexArray* arr;
        uint indexOffset;
        uint indexCount;
        uint mode;
        CSMatrix world;
        CSColor color;
        
        StaticGeometrySource(const CSVertexArray* arr, uint indexOffset, uint indexCount, uint mode, const CSMatrix& world, const CSColor& color);
        ~StaticGeometrySource();
    };
    struct GeometryInstance {
        Geometry geometry;
        const CSRootImage* image;
        CSArray<CSRenderBounds>* renderBounds;
        CSArray<DynamicGeometrySource>* dynamicSources;
        CSArray<StaticGeometrySource>* staticSources;
        
        GeometryInstance(const Geometry& geometry, const CSRootImage* image);
        ~GeometryInstance();
    };
    struct State {
        CSMatrix world;
        CSColor color;
		CSArray<CSMatrix>* worldReserved;
		CSArray<CSColor>* colorReserved;
        const CSFont* font;
        CSColor fontColors[4];
        Geometry geometry;
        CSRenderBounds stencilBounds;
        bool usingStencil;
        bool usingRenderOrder;
        bool mark;
        State* prev;
        
        State(CSRenderTarget* target, State* prev);
        ~State();
        
        void reset(CSRenderTarget* target);
    };
    CSRenderTarget* _target;
    State* _state;
    CSArray<GeometryInstance>* _instances;
    CSRenderBounds _renderBounds;
	uint _vao;
	uint _vbo;
	uint _ibo;
	CSArray<CSVertex>* _vertices;
	CSArray<CSVertex>* _accumVertices;
	CSArray<ushort>* _indices;
	uint _accumIndexCount;
	bool _began;
    bool _dirtyOffscreen;
    CSBlendMode _lastBlendMode;
    CSCullMode _lastCullMode;
    CSDepthMode _lastDepthMode;
    CSStencilMode _lastStencilMode;
    byte _lastStencilDepth;
    bool _lastUsingScissor;
    CSRect _lastScissor;
    float _lastLineWidth;
    uint _validation;
public:
    CSGraphics(CSRenderTarget* target);
private:
    ~CSGraphics();
public:
    static inline CSGraphics* graphics(CSRenderTarget* target) {
        return autorelease(new CSGraphics(target));
    }
    
public:
    //state
    void validate();
    
    static void invalidate(uint validation);
#ifdef CDK_IMPL
    static void invalidate(const CSRenderTarget* target, uint validation);
#endif
    void clear(const CSColor& color);
    void clearDepth();
    void clearStencil();
private:
    void setup(CSRenderProgram* program, const Geometry& geometry, const CSRootImage* image);
    void render(const GeometryInstance& instance, bool background, bool foreground);
    void render();
public:
    void commit();
    void flush();
    void reset();
    void push(bool mark = false);
    void pop(bool mark = false);
    
    uint pushCount() const;
    
    inline CSRenderTarget* target() {
        return _target;
    }
    inline const CSRenderTarget* target() const {
        return _target;
    }
    
public:
    //transform
    inline void setCamera(const CSCamera& camera) {
        _state->geometry.camera = camera;
    }
    inline CSCamera& camera() {
        return _state->geometry.camera;
    }
    inline const CSCamera& camera() const {
        return _state->geometry.camera;
    }
    
    inline void setWorld(const CSMatrix& world) {
        _state->world = world;
    }
    inline CSMatrix& world() {
        return _state->world;
    }
    inline const CSMatrix& world() const {
        return _state->world;
    }
    
    inline void transform(const CSMatrix& world) {
        _state->world = world * _state->world;
    }
    inline void translate(const CSVector3& v) {
        transform(CSMatrix::translation(v));
    }
    inline void rotateYawPitchRoll(float yaw, float pitch, float roll) {
        transform(CSMatrix::rotationYawPitchRoll(yaw, pitch, roll));
    }
    inline void rotateAxis(const CSVector3& axis, float v) {
        transform(CSMatrix::rotationAxis(axis, v));
    }
    inline void rotateX(float v) {
        transform(CSMatrix::rotationX(v));
    }
    inline void rotateY(float v) {
        transform(CSMatrix::rotationY(v));
    }
    inline void rotateZ(float v) {
        transform(CSMatrix::rotationZ(v));
    }
    inline void scale(const CSVector3 v) {
        transform(CSMatrix::scaling(v));
    }
    inline void scale(float v) {
        transform(CSMatrix::scaling(v));
    }
	void pushTransform();
	void popTransform();

public:
    //properties
    void setColor(const CSColor& color, bool inherit = true);
    inline const CSColor& color() const {
        return _state->color;
    }
	void pushColor();
	void popColor();
    
    void setLightness(float lightness, bool inherit = true);
    inline float lightness() const {
        return _state->geometry.var.lightness;
    }
    
    void setBrightness(float brightness, bool inherit = true);
    inline float brightness() const {
        return _state->geometry.var.brightness;
    }
    
    void setContrast(float contrast, bool inherit = true);
    inline float contrast() const {
        return _state->geometry.var.contrast;
    }
    
    void setSaturation(float saturation, bool inherit = true);
    inline float saturation() const {
        return _state->geometry.var.saturation;
    }
private:
    void applyBlendMode();
public:
    inline void setBlendMode(CSBlendMode blendMode) {
        _state->geometry.var.blendMode = blendMode;
    }
    inline CSBlendMode blendMode() const {
        return _state->geometry.var.blendMode;
    }
    
    inline void setMaskingMode(CSMaskingMode maskingMode) {
        _state->geometry.var.maskingMode = maskingMode;
    }
    inline CSMaskingMode maskingMode() const {
        return _state->geometry.var.maskingMode;
    }
    
    inline void setFont(const CSFont* font) {
        retain(_state->font, font);
    }
    inline const CSFont* font() const {
        return _state->font;
    }
    
    static void setDefaultFont(const CSFont* font);
    static const CSFont* defaultFont();
    
    inline void setFontColorV(const CSColor& topColor, const CSColor& bottomColor) {
        _state->fontColors[0] = _state->fontColors[1] = topColor;
        _state->fontColors[2] = _state->fontColors[3] = bottomColor;
    }
    inline void setFontColorH(const CSColor& leftColor, const CSColor& rightColor) {
        _state->fontColors[0] = _state->fontColors[2] = leftColor;
        _state->fontColors[1] = _state->fontColors[3] = rightColor;
    }
    inline void setFontColor(int i, const CSColor& color) {
        _state->fontColors[i] = color;
    }
    inline void resetFontColor() {
        _state->fontColors[0] = _state->fontColors[1] = _state->fontColors[2] = _state->fontColors[3] = CSColor::White;
    }
    inline const CSColor& fontColor(int i) {
        return _state->fontColors[i];
    }
    
    void setConvolution(const CSConvolution& convolution, bool inherit = true);
    inline const CSConvolution& convolution() const {
        return _state->geometry.convolution;
    }
    
    void setStrokeColor(const CSColor& strokeColor);
    inline const CSColor& strokeColor() const {
        return _state->geometry.var.sub.strokeColor;
    }
    
    inline void setStrokeMode(CSStrokeMode mode) {
        _state->geometry.var.sub.strokeMode = mode;
    }
    inline CSStrokeMode strokeMode() const {
        return _state->geometry.var.sub.strokeMode;
    }
    
    inline void setStrokeWidth(int width) {
        _state->geometry.var.sub.strokeWidth = width;
    }
    inline int strokeWidth() const {
        return _state->geometry.var.sub.strokeWidth;
    }
    
private:
    void applyCullMode();
public:
    inline void setCullMode(CSCullMode mode) {
        _state->geometry.var.cullMode = mode;
    }
    inline CSCullMode cullMode() const {
        return _state->geometry.var.cullMode;
    }
    
private:
    void applyDepthMode();
public:
    void setDepthMode(CSDepthMode mode);
    inline CSDepthMode depthMode() const {
        return _state->geometry.var.depthMode;
    }
    inline void setDepthBias(float depthBias) {
        _state->geometry.var.sub.depthBias = depthBias;
    }
    inline float depthBias() const {
        return _state->geometry.var.sub.depthBias;
    }
private:
    void applyStencilMode();
public:
    void setStencilMode(CSStencilMode stencilMode);
    inline CSStencilMode stencilMode() const {
        return _state->geometry.var.stencilMode;
    }
private:
    void applyScissor();
public:
    void setScissor(const CSRect& scissor);
    void clearScissor();
    inline bool usingSiccor() const {
        return _state->geometry.var.usingScissor;
    }
    inline const CSRect& scissor() const {
        return _state->geometry.var.sub.scissor;
    }
    
    inline void setUsingAlphaTest(bool usingAlphaTest) {
        _state->geometry.var.usingAlphaTest = usingAlphaTest;
    }
    inline bool usingAlphaTest() const {
        return _state->geometry.var.usingAlphaTest;
    }
    inline void setAlphaTest(float alphaTest) {
        _state->geometry.var.sub.alphaTest = alphaTest;
    }
    inline float alphaTest() const {
        return _state->geometry.var.sub.alphaTest;
    }
    inline void setUsingLight(bool usingLight) {
        _state->geometry.var.usingLight = usingLight;
    }
    inline bool usingLight() const {
        return _state->geometry.var.usingLight;
    }
    inline void setUsingShadow(bool usingShadow) {
        _state->geometry.var.usingShadow = usingShadow;
    }
    inline bool usingShadow() const {
        return _state->geometry.var.usingShadow;
    }
    inline void setLight(const CSLight& light) {
        _state->geometry.var.sub.light = light;
    }
    inline CSLight& light() {
        return _state->geometry.var.sub.light;
    }
    inline const CSLight& light() const {
        return _state->geometry.var.sub.light;
    }
    inline void setMaterial(const CSMaterial& material) {
        _state->geometry.var.sub.material = material;
    }
    inline CSMaterial& material() {
        return _state->geometry.var.sub.material;
    }
    inline const CSMaterial& material() const {
        return _state->geometry.var.sub.material;
    }
    inline void setLineWidth(float lineWidth) {
        CSAssert(lineWidth >= 1.0f, "invalid paraemter");
        _state->geometry.var.lineWidth = lineWidth;
    }
    inline float lineWidth() const {
        return _state->geometry.var.lineWidth;
    }
    inline void setLayer(int layer) {
        _state->geometry.var.layer = layer;
    }
    inline int layer() const {
        return _state->geometry.var.layer;
    }
    inline void setUsingRenderOrder(bool usingRenderOrder) {
        _state->usingRenderOrder = usingRenderOrder;
    }
    inline bool usingRenderOrder() const {
        return _state->usingRenderOrder;
    }
    
private:
    //vertex
    GeometryInstance* ready(const CSRootImage* image, const CSRenderBounds* renderBounds);
public:
    void begin();
    void end(const CSRootImage* image, uint mode);
    
    inline uint vertexCount() const {
        return _vertices->count();
    }
    inline uint indexCount() const {
        return _indices->count();
    }
    void addVertex(const CSVertex& vertex);
    void addIndex(uint index);

    static void applyAlign(CSVector3& point, const CSSize& size, CSAlign align);
    static void applyAlign(CSVector2& point, const CSSize& size, CSAlign align);
public:
    //shape
    void drawPoint(const CSVector3& point);
    void drawLine(const CSVector3& point1, const CSVector3& point2);
    void drawGradientLine(const CSVector3& point1, const CSColor& color1, const CSVector3& point2, const CSColor& color2);
    void drawRect(const CSZRect& rect, bool fill);
private:
    void drawGradientRect(const CSZRect& rect, const CSColor& leftTopColor, const CSColor& rightTopColor, const CSColor& leftBottomColor, const CSColor& rightBottomColor, bool fill);
public:
    inline void drawGradientRectV(const CSZRect& rect, const CSColor& topColor, const CSColor& bottomColor, bool fill) {
        drawGradientRect(rect, topColor, topColor, bottomColor, bottomColor, fill);
    }
    inline void drawGradientRectH(const CSZRect& rect, const CSColor& leftColor, const CSColor& rightColor, bool fill) {
        drawGradientRect(rect, leftColor, rightColor, leftColor, rightColor, fill);
    }
    void drawRoundRect(const CSZRect& rect, float radius, bool fill, CSCorner corner = CSCornerAll);
private:
    void drawGradientRoundRect(const CSZRect& rect, float radius, const CSColor& leftTopColor, const CSColor& rightTopColor, const CSColor& leftBottomColor, const CSColor& rightBottomColor, bool fill);
public:
    inline void drawGradientRoundRectV(const CSZRect& rect, float radius, const CSColor& topColor, const CSColor& bottomColor, bool fill) {
        drawGradientRoundRect(rect, radius, topColor, topColor, bottomColor, bottomColor, fill);
    }
    inline void drawGradientRoundRectH(const CSZRect& rect, float radius, const CSColor& leftColor, const CSColor& rightColor, bool fill) {
        drawGradientRoundRect(rect, radius, leftColor, rightColor, leftColor, rightColor, fill);
    }
    void drawArc(const CSZRect& rect, float angle1, float angle2, bool fill);
    inline void drawCircle(const CSZRect& rect, bool fill) {
        drawArc(rect, 0, FloatTwoPi, fill);
    }
    void drawGradientArc(const CSZRect& rect, float angle1, float angle2, const CSColor& centerColor, const CSColor& surroundColor);
    void drawGradientCircle(const CSZRect& rect, const CSColor& centerColor, const CSColor& surroundColor) {
        drawGradientArc(rect, 0, FloatTwoPi, centerColor, surroundColor);
    }
    
private:
    //image
    void drawImageImpl(const CSImage* image, const CSZRect& destRect, const CSRect& frame, const CSColor* colors);
public:
    void drawImage(const CSImage* image, const CSZRect& destRect, const CSRect& srcRect);
    void drawImage(const CSImage* image, const CSZRect& destRect, bool stretch = true);
    void drawImage(const CSImage* image, CSVector3 point, CSAlign align);
    void drawImage(const CSImage* image, const CSVector3& point);
    void drawImageScaled(const CSImage* image, CSVector3 point, float scale, CSAlign align);
    void drawImageScaled(const CSImage* image, const CSVector3& point, float scale);
    void drawLineImage(const CSImage* image, float scroll, const CSVector3& src, const CSVector3& dest);
    void drawClockImage(const CSImage* image, float progress, CSVector3 point, CSAlign align);
    void drawClockImageScaled(const CSImage* image, float progress, CSVector3 point, float scale, CSAlign align);
    void drawClockImage(const CSImage* image, float progress, const CSZRect& destRect);
    void drawStretchImage(const CSImage* image, const CSZRect& destRect, float horizontal, float vertical);
    void drawStretchImage(const CSImage* image, const CSZRect& destRect);
private:
    void drawShadowFlatImageImpl(const CSImage* image, const CSZRect& destRect, const CSRect& frame, float left, float right, bool xflip, bool yflip);
public:
    void drawShadowFlatImage(const CSImage* image, const CSZRect& destRect, const CSRect& srcRect, float left = 0, float right = 0, bool xflip = false, bool yflip = false);
    void drawShadowFlatImage(const CSImage* image, const CSZRect& destRect, float left = 0, float right = 0, bool xflip = false, bool yflip = false);
    void drawShadowFlatImage(const CSImage* image, CSVector3 point, CSAlign align, float left = 0, float right = 0, bool xflip = false, bool yflip = false);
    void drawShadowFlatImage(const CSImage* image, const CSVector3& point, float left = 0, float right = 0, bool xflip = false, bool yflip = false);
private:
    void drawShadowRotateImageImpl(const CSImage* image, const CSZRect& destRect, const CSRect& frame, const CSVector2& offset, float flatness);
public:
    void drawShadowRotateImage(const CSImage* image, const CSZRect& destRect, const CSRect& srcRect, const CSVector2& offset = CSVector2::Zero, float flatness = 0);
    void drawShadowRotateImage(const CSImage* image, const CSZRect& destRect, const CSVector2& offset = CSVector2::Zero, float flatness = 0);
    void drawShadowRotateImage(const CSImage* image, CSVector3 point, CSAlign align, const CSVector2& offset = CSVector2::Zero, float flatness = 0);
    void drawShadowRotateImage(const CSImage* image, const CSVector3& point, const CSVector2& offset = CSVector2::Zero, float flatness = 0);
public:
	//string
	struct StringParam {
    private:
        const CSStringDisplay* _display;
		bool _freeDisplay;
    public:
        uint start;
        uint end;

        StringParam();
    public:
        StringParam(const char* str);
		StringParam(const CSString* str);
		StringParam(const CSString* str, uint offset, uint length);
		StringParam(const CSLocaleString* str);
		StringParam(const StringParam& other);
		~StringParam();
	private:
		StringParam operator=(const StringParam& other);
	public:
		inline operator bool() const {
			return _display != NULL;
		}
		inline bool operator !() const {
			return _display == NULL;
		}
        inline const CSStringDisplay* display() const {
            return _display;
        }
	};
private:
    struct ParagraphDisplay {
        const CSFont* font;
        CSSize size;
        enum {
            Forward,
            Visible,
            Backward,
        } visible;
    };
    static const CSArray<ParagraphDisplay>* paragraphDisplays(const StringParam& str, const CSFont* font);

    static CSSize stringSizeImpl(const StringParam& str, float width, const CSArray<ParagraphDisplay>* paraDisplays);
    static CSSize stringLineSizeImpl(const StringParam& str, uint pi, float width, const CSArray<ParagraphDisplay>* paraDisplays);
public:
    static uint stringIndex(const StringParam& str, uint ci);
	static CSSize stringSize(const StringParam& str, const CSFont* font, float width = CSStringWidthUnlimited);
	CSSize stringSize(const StringParam& str, float width = CSStringWidthUnlimited) const;
    static uint stringCursor(const StringParam& str, const CSFont* font, const CSVector2& target, float width = CSStringWidthUnlimited);
	uint stringCursor(const StringParam& str, const CSVector2& target, float width = CSStringWidthUnlimited) const;
	static CSVector2 stringPosition(const StringParam& str, const CSFont* font, float width = CSStringWidthUnlimited);
	CSVector2 stringPosition(const StringParam& str, float width = CSStringWidthUnlimited) const;
    static CSString* shrinkString(const StringParam& str, const CSFont* font, float scroll, float width = CSStringWidthUnlimited);
private:
    void drawStringCharacter(const CSImage* image, const CSVector3& pos, const CSRootImage*& root);
    void drawStringCharacterEnd(const CSRootImage*& root);
    void drawStringImpl(const StringParam& str, const CSZRect& destRect, float scroll, const CSArray<ParagraphDisplay>* paraDisplays);
public:
    void drawStringParagraphs(const StringParam& str, const CSVector3& point);
    void drawStringScrolled(const StringParam& str, const CSZRect& destRect, float scroll);
    inline void drawString(const StringParam& str, const CSZRect& destRect) {
        drawStringScrolled(str, destRect, 0);
    }
    void drawString(const StringParam& str, CSZRect destRect, CSAlign align);
    float drawString(const StringParam& str, const CSVector3& point, float width = CSStringWidthUnlimited);
    float drawString(const StringParam& str, CSVector3 point, CSAlign align, float width = CSStringWidthUnlimited);
    void drawStringScaled(const StringParam& str, const CSZRect& destRect);
    void drawStringScaled(const StringParam& str, CSZRect destRect, CSAlign align);
    void drawStringScaled(const StringParam& str, const CSVector3& point, float width);
    void drawStringScaled(const StringParam& str, CSVector3 point, CSAlign align, float width);
    void drawStringTruncated(const StringParam& str, const CSVector3& point, float width);
    void drawStringTruncated(StringParam str, CSVector3 point, CSAlign align, float width);
public:
    //globalization
    float drawDigit(int value, bool comma, const CSVector3& point);
    float drawDigit(int value, bool comma, const CSVector3& point, CSAlign align);
    float drawDigitImages(uint value, const CSArray<CSImage>* images, bool comma, uint offset, const CSVector3& point, CSAlign align = CSAlignNone, float spacing = 0);
public:
    //vertices
    void drawVertices(const CSRootImage* image, const CSVertexArray* arr, uint indexOffset, uint indexCount, uint mode);
    inline void drawVertices(const CSVertexArray* arr, uint mode) {
        drawVertices(NULL, arr, 0, arr->indexCount(), mode);
    }
    inline void drawVertices(const CSVertexArray* arr, uint indexOffset, uint indexCount, uint mode) {
        drawVertices(NULL, arr, indexOffset, indexCount, mode);
    }
    inline void drawVertices(const CSRootImage* image, const CSVertexArray* arr, uint mode) {
        drawVertices(image, arr, 0, arr->indexCount(), mode);
    }
    
public:
    //grab
    CSRootImage* grab();
    CSRootImage* grab(const CSRect& rect);
    
public:
    //FX
    void shockwave(const CSVector3& center, float range, float thickness, float progress);
    void lens(const CSVector3& center, float range, bool convex);
    void swirl(const CSVector3& center, float radius, float angle);
    void pixellate(const CSZRect& rect, float fraction);
    void kuwahara(const CSZRect& rect, int radius);
    void blur(const CSZRect& rect, float weight);
	void blur(const CSZRect& rect, const CSVector2& dir);
	void blur(const CSZRect& rect, const CSVector3& center, float range);
    
    inline void resetOffscreen() {
        _dirtyOffscreen = true;
    }

private:
    //sprite
    void sprColor(const byte*& cursor);
    void sprLightness(const byte*& cursor);
    void sprBrightness(const byte*& cursor);
    void sprContrast(const byte*& cursor);
    void sprSaturation(const byte*& cursor);
    void sprBlendMode(const byte*& cursor);
    void sprMaskingMode(const byte*& cursor);
    void sprConvolution(const byte*& cursor);
    void sprStroke(const byte*& cursor);
    void sprFont(const byte*& cursor);
    void sprFontColor(const byte*& cursor);
    void sprMaterial(const byte*& cursor);
    void sprTranslate(const byte*& cursor);
    void sprScale(const byte*& cursor);
    void sprRotate(const byte*& cursor);
    void sprBillboard(const byte*& cursor);
    void sprImage(const byte*& cursor, bool shadow, CSResourceDelegate* resourceDelegate);
    void sprLine(const byte*& cursor);
    void sprRect(const byte*& cursor);
    void sprArc(const byte*& cursor);
    void sprGradientLine(const byte*& cursor);
    void sprGradientRect(const byte*& cursor);
    void sprGradientArc(const byte*& cursor);
    void sprString(const byte*& cursor, CSResourceDelegate* resourceDelegate);
    void sprShockwave(const byte*& cursor);
    void sprLens(const byte*& cursor);
    void sprSwirl(const byte*& cursor);
    void sprBlur(const byte*& cursor);
public:
    void drawSprite(const CSArray<byte>* sprite, bool shadow, CSResourceDelegate* resourceDelegate = NULL, CSSpriteDelegate* spriteDelegate = NULL);

    static void preloadSprite(const CSArray<byte>* sprite, CSResourceDelegate* resourceDelegate);
};

#endif /* defined(__CDK__CSGraphics__) */
