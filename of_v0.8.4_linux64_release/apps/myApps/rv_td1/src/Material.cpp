#include "Material.h"

Material::Material(string filename)
{
  loadMipMapTexture(filename, 4);
}

Material::~Material()
{
}

void Material::bind()
{
    _tex.bind();
    ofSetColor(255, 255, 255);
    ofFill();
}

void Material::unbind()
{
    _tex.unbind();
}

void Material::loadMipMapTexture(string inPath, float inAnisotropy ) {
    ofPixels pix;
    ofLoadImage(pix, inPath);

    _tex.allocate( pix.getWidth(), pix.getHeight(), ofGetGlInternalFormat(pix) );

    ofTextureData& texData = _tex.texData;

    if (texData.textureTarget == GL_TEXTURE_RECTANGLE_ARB){
        texData.tex_t = pix.getWidth();
        texData.tex_u = pix.getHeight();
    } else {
        texData.tex_t = (float)(pix.getWidth()) / (float)texData.tex_w;
        texData.tex_u = (float)(pix.getHeight()) / (float)texData.tex_h;
    }

    ofSetPixelStorei(pix.getWidth(),pix.getBytesPerChannel(),pix.getNumChannels());

    glBindTexture( texData.textureTarget, texData.textureID);
    glTexImage2D( texData.textureTarget, 0, texData.glTypeInternal, pix.getWidth(), pix.getHeight(), 0, ofGetGlFormat(pix), ofGetGlType(pix), pix.getPixels());
    glHint(GL_GENERATE_MIPMAP_HINT, GL_NICEST);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glEnable(texData.textureTarget);
    glGenerateMipmap(texData.textureTarget);
    glTexParameteri( texData.textureTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri( texData.textureTarget, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    // 1 means it does nothing
    if( inAnisotropy >= 1 ) {
        glTexParameterf( texData.textureTarget, GL_TEXTURE_MAX_ANISOTROPY_EXT, (GLfloat)inAnisotropy);
    }

    glDisable( texData.textureTarget );
}
