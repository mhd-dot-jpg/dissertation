#include <iostream>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include "glm/gtc/matrix_transform.hpp"
#include "stb_image.h"

#include "model.h"

GLuint loadTexture( const char* filename, bool flip ){

    stbi_set_flip_vertically_on_load( flip );

    int width, height, channels;
    unsigned char* data = stbi_load( filename, &width, &height, &channels, STBI_rgb_alpha);

    GLuint textureBuffer;

    glGenTextures( 1, &textureBuffer);
    glBindTexture( GL_TEXTURE_2D, textureBuffer);
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT ); 
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data );
    glGenerateMipmap( GL_TEXTURE_2D );

    glBindTexture( GL_TEXTURE_2D, 0 );

    stbi_image_free( data );

    return textureBuffer;

}

Mesh::Mesh( aiMesh* mesh ){

    for(unsigned int i = 0; i < mesh->mNumVertices; i++){
        aiVector3D position = mesh->mVertices[i];
        aiVector3D normal = mesh->mNormals[i];
        Vertex v;
        v.position = { position.x, position.y, position.z };
        v.normal = { normal.x, normal.y, normal.z };
        if(mesh->HasTextureCoords( 0 )){
            aiVector3D uv = mesh->mTextureCoords[0][i];
            v.uv = { uv.x, uv.y };
        }
        if(mesh->HasTangentsAndBitangents()){
            aiVector3D tangent = mesh->mTangents[i];
            aiVector3D bitangent = mesh->mBitangents[i];
            v.tangent = { tangent.x, tangent.y, tangent.z };
            v.bitangent = { bitangent.x, bitangent.y, bitangent.z };
        }
        vertices.push_back( v );
    }

    for(unsigned int i = 0; i < mesh->mNumFaces; i++){
        aiFace face = mesh->mFaces[i];
        for(unsigned int j = 0; j < face.mNumIndices; j++){
            indices.push_back( face.mIndices[j] );
        }
    }

    materialIndex = mesh->mMaterialIndex;
    vao = 0;

}

GLuint loadMaterial( aiMaterial* material ){

    if(material->GetTextureCount( aiTextureType_DIFFUSE ) == 0){
        return 0;
    }

    aiString texPath;
    material->GetTexture( aiTextureType_DIFFUSE, 0, &texPath );
    GLuint texBuffer = loadTexture( texPath.C_Str(), true );
    return texBuffer;

}

void generateBuffers( Mesh& mesh ){

    GLuint vao, vbo, ebo;
    glGenVertexArrays( 1, &vao );
    glBindVertexArray( vao );

    glGenBuffers( 1, &vbo );
    glBindBuffer( GL_ARRAY_BUFFER, vbo );
    glGenBuffers( 1, &ebo );

    glBufferData( GL_ARRAY_BUFFER, mesh.vertices.size() * sizeof(Vertex), mesh.vertices.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray( 0 );
    glVertexAttribPointer( 0, glm::vec3::length(), GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof( Vertex, position ));
    glEnableVertexAttribArray( 1 );
    glVertexAttribPointer( 1, glm::vec3::length(), GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof( Vertex, normal ));
    glEnableVertexAttribArray( 2 );
    glVertexAttribPointer( 2, glm::vec3::length(), GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof( Vertex, tangent ));
    glEnableVertexAttribArray( 3 );
    glVertexAttribPointer( 3, glm::vec3::length(), GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof( Vertex, bitangent ));
    glEnableVertexAttribArray( 4 );
    glVertexAttribPointer( 4, glm::vec2::length(), GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof( Vertex, uv ));

    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, ebo );
    glBufferData( GL_ELEMENT_ARRAY_BUFFER, mesh.indices.size() * sizeof(GLuint), mesh.indices.data(), GL_STATIC_DRAW);

    mesh.vao = vao;

}

Model::Model( const std::string& filename ){

    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile( filename, aiProcess_Triangulate | aiProcess_CalcTangentSpace );

    if(!scene){
        fprintf( stderr, "Assimp error: %s\n", importer.GetErrorString() );
        return;
    }

    for(unsigned int i = 0; i < scene->mNumMeshes; i++){
        meshes.push_back( Mesh( scene->mMeshes[i] ) );
    }

    for(unsigned int i = 0; i < scene->mNumMaterials; i++){
        materials.push_back( loadMaterial( scene->mMaterials[i] ) ); 
    }

    for(Mesh& meshIter : meshes){
        generateBuffers( meshIter );
    }

    transform = glm::scale( glm::mat4( 1.0f ), glm::vec3( 0.01f, 0.01f, 0.01f ) );

}
