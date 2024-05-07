/**
 * \file
 * \author Rudy Castan
 * \date 2024 Spring
 * \par CS250 Computer Graphics II
 * \copyright DigiPen Institute of Technology
 */
#include "opengl/GL.hpp"

#include <GL/glew.h>
#include <cassert>
#include <iostream>
#include <sstream>
#include <string>

#if defined(_WIN32)
#    define WIN32_LEAN_AND_MEAN
#    define NOGDICAPMASKS     // CC_*, LC_*, PC_*, CP_*, TC_*, RC_
#    define NOVIRTUALKEYCODES // VK_*
#    define NOWINMESSAGES     // WM_*, EM_*, LB_*, CB_*
#    define NOWINSTYLES       // WS_*, CS_*, ES_*, LBS_*, SBS_*, CBS_*
#    define NOSYSMETRICS      // SM_*
#    define NOMENUS           // MF_*
#    define NOICONS           // IDI_*
#    define NOKEYSTATES       // MK_*
#    define NOSYSCOMMANDS     // SC_*
#    define NORASTEROPS       // Binary and Tertiary raster ops
#    define NOSHOWWINDOW      // SW_*
#    define OEMRESOURCE       // OEM Resource values
#    define NOATOM            // Atom Manager routines
#    define NOCLIPBOARD       // Clipboard routines
#    define NOCOLOR           // Screen colors
#    define NOCTLMGR          // Control and Dialog routines
#    define NODRAWTEXT        // DrawText() and DT_*
#    define NOGDI             // All GDI defines and routines
#    define NOKERNEL          // All KERNEL defines and routines
#    define NOUSER            // All USER defines and routines
#    define NONLS             // All NLS defines and routines
#    define NOMB              // MB_* and MessageBox()
#    define NOMEMMGR          // GMEM_*, LMEM_*, GHND, LHND, associated routines
#    define NOMETAFILE        // typedef METAFILEPICT
#    define NOMINMAX          // Macros min(a,b) and max(a,b)
#    define NOMSG             // typedef MSG and associated routines
#    define NOOPENFILE        // OpenFile(), OemToAnsi, AnsiToOem, and OF_*
#    define NOSCROLL          // SB_* and scrolling routines
#    define NOSERVICE         // All Service Controller routines, SERVICE_ equates, etc.
#    define NOSOUND           // Sound driver routines
#    define NOTEXTMETRIC      // typedef TEXTMETRIC and associated routines
#    define NOWH              // SetWindowsHook and WH_*
#    define NOWINOFFSETS      // GWL_*, GCL_*, associated routines
#    define NOCOMM            // COMM driver routines
#    define NOKANJI           // Kanji support stuff.
#    define NOHELP            // Help engine interface.
#    define NOPROFILER        // Profiler interface.
#    define NODEFERWINDOWPOS  // DeferWindowPos routines
#    define NOMCX             // Modem Configuration Extensions
#    include <Windows.h>
#    undef MemoryBarrier
#endif

#if defined(CS250_USE_SOURCE_LOCATION)
#    include <source_location>
#    define VOID_SOURCE_LOCATION const std::source_location caller_location
#    define SOURCE_LOCATION      , VOID_SOURCE_LOCATION
#    define glCheck(expression)                                                                                                                                                    \
        expression;                                                                                                                                                                \
        glCheckError(caller_location.file_name(), caller_location.line(), caller_location.function_name(), #expression)

namespace
{
    inline void glCheckError(const char* file, unsigned line, const char* function_name, const char* opengl_function)
    {
        GLenum errorCode = glGetError();

        if (errorCode == GL_NO_ERROR)
            return;

        const std::string  fileString  = file;
        std::string        error       = "Unknown error";
        std::string        description = "No description";
        std::ostringstream serr;

        serr << "OpenGL call " << opengl_function << " failed in " << fileString.substr(fileString.find_last_of("\\/") + 1) << "(" << line << ")."
             << "\nwithin Function:\n   " << function_name << "\nError description:\n   ";
        int loop_limit = 0;
        while (errorCode != GL_NO_ERROR && loop_limit < 3)
        {
            ++loop_limit;
            switch (errorCode)
            {
                case GL_INVALID_ENUM:
                    {
                        error       = "GL_INVALID_ENUM";
                        description = "An unacceptable value has been specified for an enumerated argument.";
                        break;
                    }

                case GL_INVALID_VALUE:
                    {
                        error       = "GL_INVALID_VALUE";
                        description = "A numeric argument is out of range.";
                        break;
                    }

                case GL_INVALID_OPERATION:
                    {
                        error       = "GL_INVALID_OPERATION";
                        description = "The specified operation is not allowed in the current state.";
                        break;
                    }

                case GL_STACK_OVERFLOW:
                    {
                        error       = "GL_STACK_OVERFLOW";
                        description = "This command would cause a stack overflow.";
                        break;
                    }

                case GL_STACK_UNDERFLOW:
                    {
                        error       = "GL_STACK_UNDERFLOW";
                        description = "This command would cause a stack underflow.";
                        break;
                    }

                case GL_OUT_OF_MEMORY:
                    {
                        error       = "GL_OUT_OF_MEMORY";
                        description = "There is not enough memory left to execute the command.";
                        break;
                    }

                default:
                    {
                        error += " " + std::to_string(errorCode);
                        break;
                    }
            }

            serr << error << "\n   " << description << "\n\n";
            errorCode = glGetError();
        }
        std::cerr << serr.str();
#    if defined(_WIN32)
        MessageBox(nullptr, serr.str().c_str(), "OpenGL Error", MB_ICONERROR | MB_OK);
#    endif
        assert(false);
    }
}
#else
#    define SOURCE_LOCATION
#    define VOID_SOURCE_LOCATION void
#    define glCheck(expression)  expression
#endif


namespace GL
{
    const GLubyte* GetString(GLenum name SOURCE_LOCATION)
    {
        glCheck(const auto the_string = glGetString(name));
        return the_string;
    }

    GLint GetAttribLocation(GLuint program, const GLchar* name SOURCE_LOCATION)
    {
        glCheck(const auto location = glGetAttribLocation(program, name));
        return location;
    }

    GLint GetUniformLocation(GLuint program, const GLchar* name SOURCE_LOCATION)
    {
        glCheck(const auto location = glGetUniformLocation(program, name));
        return location;
    }

    GLuint CreateProgram(VOID_SOURCE_LOCATION)
    {
        glCheck(const auto program = glCreateProgram());
        return program;
    }

    GLuint CreateShader(GLenum shaderType SOURCE_LOCATION)
    {
        glCheck(const auto shader = glCreateShader(shaderType));
        return shader;
    }

    void ActiveTexture(GLenum texture SOURCE_LOCATION)
    {
        glCheck(glActiveTexture(texture));
    }

    void AttachShader(GLuint program, GLuint shader SOURCE_LOCATION)
    {
        glCheck(glAttachShader(program, shader));
    }

    void BindBuffer(GLenum target, GLuint buffer SOURCE_LOCATION)
    {
        glCheck(glBindBuffer(target, buffer));
    }

    void BindTexture(GLenum target, GLuint texture SOURCE_LOCATION)
    {
        glCheck(glBindTexture(target, texture));
    }

    void BufferData(GLenum target, GLsizeiptr size, const GLvoid* data, GLenum usage SOURCE_LOCATION)
    {
        glCheck(glBufferData(target, size, data, usage));
    }

    void BufferSubData(GLenum target, GLintptr offset, GLsizeiptr size, const GLvoid* data SOURCE_LOCATION)
    {
        glCheck(glBufferSubData(target, offset, size, data));
    }

    void Clear(GLbitfield mask SOURCE_LOCATION)
    {
        glCheck(glClear(mask));
    }

    void ClearColor(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha SOURCE_LOCATION)
    {
        glCheck(glClearColor(red, green, blue, alpha));
    }

    void CompileShader(GLuint shader SOURCE_LOCATION)
    {
        glCheck(glCompileShader(shader));
    }

    void CullFace(GLenum mode SOURCE_LOCATION)
    {
        glCheck(glCullFace(mode));
    }

    void DeleteBuffers(GLsizei n, const GLuint* buffers SOURCE_LOCATION)
    {
        glCheck(glDeleteBuffers(n, buffers));
    }

    void DeleteProgram(GLuint program SOURCE_LOCATION)
    {
        glCheck(glDeleteProgram(program));
    }

    void DeleteShader(GLuint shader SOURCE_LOCATION)
    {
        glCheck(glDeleteShader(shader));
    }

    void DeleteTextures(GLsizei n, const GLuint* textures SOURCE_LOCATION)
    {
        glCheck(glDeleteTextures(n, textures));
    }

    void DepthMask(GLboolean flag SOURCE_LOCATION)
    {
        glCheck(glDepthMask(flag));
    }

    void Disable(GLenum cap SOURCE_LOCATION)
    {
        glCheck(glDisable(cap));
    }

    void DrawArrays(GLenum mode, GLint first, GLsizei count SOURCE_LOCATION)
    {
        glCheck(glDrawArrays(mode, first, count));
    }

    void DrawBuffers(GLsizei n, const GLenum* bufs SOURCE_LOCATION)
    {
        glCheck(glDrawBuffers(n, bufs));
    }

    void DrawElements(GLenum mode, GLsizei count, GLenum type, const GLvoid* indices SOURCE_LOCATION)
    {
        glCheck(glDrawElements(mode, count, type, indices));
    }

    void Enable(GLenum cap SOURCE_LOCATION)
    {
        glCheck(glEnable(cap));
    }

    void EnableVertexAttribArray(GLuint index SOURCE_LOCATION)
    {
        glCheck(glEnableVertexAttribArray(index));
    }

    void FrontFace(GLenum mode SOURCE_LOCATION)
    {
        glCheck(glFrontFace(mode));
    }

    void GenBuffers(GLsizei n, GLuint* buffers SOURCE_LOCATION)
    {
        glCheck(glGenBuffers(n, buffers));
    }

    void GenTextures(GLsizei n, GLuint* textures SOURCE_LOCATION)
    {
        glCheck(glGenTextures(n, textures));
    }

    void GetActiveAttrib(GLuint program, GLuint index, GLsizei bufSize, GLsizei* length, GLint* size, GLenum* type, GLchar* name SOURCE_LOCATION)
    {
        glCheck(glGetActiveAttrib(program, index, bufSize, length, size, type, name));
    }

    void GetActiveUniform(GLuint program, GLuint index, GLsizei bufSize, GLsizei* length, GLint* size, GLenum* type, GLchar* name SOURCE_LOCATION)
    {
        glCheck(glGetActiveUniform(program, index, bufSize, length, size, type, name));
    }

    void GetBooleanv(GLenum pname, GLboolean* data SOURCE_LOCATION)
    {
        glCheck(glGetBooleanv(pname, data));
    }

    void GetIntegerv(GLenum pname, GLint* data SOURCE_LOCATION)
    {
        glCheck(glGetIntegerv(pname, data));
    }

    void GetProgramInfoLog(GLuint program, GLsizei maxLength, GLsizei* length, GLchar* infoLog SOURCE_LOCATION)
    {
        glCheck(glGetProgramInfoLog(program, maxLength, length, infoLog));
    }

    void GetProgramiv(GLuint program, GLenum pname, GLint* params SOURCE_LOCATION)
    {
        glCheck(glGetProgramiv(program, pname, params));
    }

    void GetShaderInfoLog(GLuint shader, GLsizei maxLength, GLsizei* length, GLchar* infoLog SOURCE_LOCATION)
    {
        glCheck(glGetShaderInfoLog(shader, maxLength, length, infoLog));
    }

    void GetShaderiv(GLuint shader, GLenum pname, GLint* params SOURCE_LOCATION)
    {
        glCheck(glGetShaderiv(shader, pname, params));
    }

    void GetUniformfv(GLuint program, GLint location, GLfloat* params SOURCE_LOCATION)
    {
        glCheck(glGetUniformfv(program, location, params));
    }

    void GetUniformiv(GLuint program, GLint location, GLint* params SOURCE_LOCATION)
    {
        glCheck(glGetUniformiv(program, location, params));
    }

    void GetUniformuiv(GLuint program, GLint location, GLuint* params SOURCE_LOCATION)
    {
        glCheck(glGetUniformuiv(program, location, params));
    }

    void LinkProgram(GLuint program SOURCE_LOCATION)
    {
        glCheck(glLinkProgram(program));
    }

    void PolygonOffset(GLfloat factor, GLfloat units SOURCE_LOCATION)
    {
        glCheck(glPolygonOffset(factor, units));
    }

    void ShaderSource(GLuint shader, GLsizei count, const GLchar** string, const GLint* length SOURCE_LOCATION)
    {
        glCheck(glShaderSource(shader, count, string, length));
    }

    void TexImage2D(GLenum target, GLint level, GLint internalFormat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid* data SOURCE_LOCATION)
    {
        glCheck(glTexImage2D(target, level, internalFormat, width, height, border, format, type, data));
    }

    void TexParameterfv(GLenum target, GLenum pname, const GLfloat* params SOURCE_LOCATION)
    {
        glCheck(glTexParameterfv(target, pname, params));
    }

    void TexParameteri(GLenum target, GLenum pname, GLint param SOURCE_LOCATION)
    {
        glCheck(glTexParameteri(target, pname, param));
    }

    void Uniform1f(GLint location, GLfloat v0 SOURCE_LOCATION)
    {
        glCheck(glUniform1f(location, v0));
    }

    void Uniform1i(GLint location, GLint v0 SOURCE_LOCATION)
    {
        glCheck(glUniform1i(location, v0));
    }

    void Uniform1ui(GLint location, GLuint v0 SOURCE_LOCATION)
    {
        glCheck(glUniform1ui(location, v0));
    }

    void Uniform2f(GLint location, GLfloat v0, GLfloat v1 SOURCE_LOCATION)
    {
        glCheck(glUniform2f(location, v0, v1));
    }

    void Uniform2fv(GLint location, GLsizei count, const GLfloat* value SOURCE_LOCATION)
    {
        glCheck(glUniform2fv(location, count, value));
    }

    void Uniform2i(GLint location, GLint v0, GLint v1 SOURCE_LOCATION)
    {
        glCheck(glUniform2i(location, v0, v1));
    }

    void Uniform2ui(GLint location, GLuint v0, GLuint v1 SOURCE_LOCATION)
    {
        glCheck(glUniform2ui(location, v0, v1));
    }

    void Uniform3f(GLint location, GLfloat v0, GLfloat v1, GLfloat v2 SOURCE_LOCATION)
    {
        glCheck(glUniform3f(location, v0, v1, v2));
    }

    void Uniform3fv(GLint location, GLsizei count, const GLfloat* value SOURCE_LOCATION)
    {
        glCheck(glUniform3fv(location, count, value));
    }

    void Uniform3i(GLint location, GLint v0, GLint v1, GLint v2 SOURCE_LOCATION)
    {
        glCheck(glUniform3i(location, v0, v1, v2));
    }

    void Uniform3ui(GLint location, GLuint v0, GLuint v1, GLuint v2 SOURCE_LOCATION)
    {
        glCheck(glUniform3ui(location, v0, v1, v2));
    }

    void Uniform4f(GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3 SOURCE_LOCATION)
    {
        glCheck(glUniform4f(location, v0, v1, v2, v3));
    }

    void Uniform4fv(GLint location, GLsizei count, const GLfloat* value SOURCE_LOCATION)
    {
        glCheck(glUniform4fv(location, count, value));
    }

    void Uniform4i(GLint location, GLint v0, GLint v1, GLint v2, GLint v3 SOURCE_LOCATION)
    {
        glCheck(glUniform4i(location, v0, v1, v2, v3));
    }

    void Uniform4ui(GLint location, GLuint v0, GLuint v1, GLuint v2, GLuint v3 SOURCE_LOCATION)
    {
        glCheck(glUniform4ui(location, v0, v1, v2, v3));
    }

    void UniformMatrix2fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value SOURCE_LOCATION)
    {
        glCheck(glUniformMatrix2fv(location, count, transpose, value));
    }

    void UniformMatrix2x3fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value SOURCE_LOCATION)
    {
        glCheck(glUniformMatrix2x3fv(location, count, transpose, value));
    }

    void UniformMatrix2x4fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value SOURCE_LOCATION)
    {
        glCheck(glUniformMatrix2x4fv(location, count, transpose, value));
    }

    void UniformMatrix3fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value SOURCE_LOCATION)
    {
        glCheck(glUniformMatrix3fv(location, count, transpose, value));
    }

    void UniformMatrix3x2fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value SOURCE_LOCATION)
    {
        glCheck(glUniformMatrix3x2fv(location, count, transpose, value));
    }

    void UniformMatrix3x4fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value SOURCE_LOCATION)
    {
        glCheck(glUniformMatrix3x4fv(location, count, transpose, value));
    }

    void UniformMatrix4fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value SOURCE_LOCATION)
    {
        glCheck(glUniformMatrix4fv(location, count, transpose, value));
    }

    void UniformMatrix4x2fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value SOURCE_LOCATION)
    {
        glCheck(glUniformMatrix4x2fv(location, count, transpose, value));
    }

    void UniformMatrix4x3fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value SOURCE_LOCATION)
    {
        glCheck(glUniformMatrix4x3fv(location, count, transpose, value));
    }

    void UseProgram(GLuint program SOURCE_LOCATION)
    {
        glCheck(glUseProgram(program));
    }

    void ValidateProgram(GLuint program SOURCE_LOCATION)
    {
        glCheck(glValidateProgram(program));
    }

    void VertexAttribPointer(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid* pointer SOURCE_LOCATION)
    {
        glCheck(glVertexAttribPointer(index, size, type, normalized, stride, pointer));
    }

    void Viewport(GLint x, GLint y, GLsizei width, GLsizei height SOURCE_LOCATION)
    {
        glCheck(glViewport(x, y, width, height));
    }

    GLenum CheckFramebufferStatus(GLenum target SOURCE_LOCATION)
    {
        glCheck(const GLenum status = glCheckFramebufferStatus(target));
        return status;
    }

    void BindFramebuffer(GLenum target, GLuint framebuffer SOURCE_LOCATION)
    {
        glCheck(glBindFramebuffer(target, framebuffer));
    }

    void BindVertexArray(GLuint array SOURCE_LOCATION)
    {
        glCheck(glBindVertexArray(array));
    }

    void DeleteFramebuffers(GLsizei n, GLuint* framebuffers SOURCE_LOCATION)
    {
        glCheck(glDeleteFramebuffers(n, framebuffers));
    }

    void DeleteVertexArrays(GLsizei n, const GLuint* arrays SOURCE_LOCATION)
    {
        glCheck(glDeleteVertexArrays(n, arrays));
    }

    void FramebufferTexture2D(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level SOURCE_LOCATION)
    {
        glCheck(glFramebufferTexture2D(target, attachment, textarget, texture, level));
    }

    void GenFramebuffers(GLsizei n, GLuint* framebuffers SOURCE_LOCATION)
    {
        glCheck(glGenFramebuffers(n, framebuffers));
    }

    void GenVertexArrays(GLsizei n, GLuint* arrays SOURCE_LOCATION)
    {
        glCheck(glGenVertexArrays(n, arrays));
    }

    void TexStorage2D(GLenum target, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height SOURCE_LOCATION)
    {
        glCheck(glTexStorage2D(target, levels, internalformat, width, height));
    }

#if !defined(OPENGL_ES3_ONLY)

    void PatchParameteri(GLenum pname, GLint value SOURCE_LOCATION)
    {
        glCheck(glPatchParameteri(pname, value));
    }

    void BindImageTexture(GLuint unit, GLuint texture, GLint level, GLboolean layered, GLint layer, GLenum access, GLenum format SOURCE_LOCATION)
    {
        glCheck(glBindImageTexture(unit, texture, level, layered, layer, access, format));
    }

    void MemoryBarrier(GLbitfield barriers SOURCE_LOCATION)
    {
        glCheck(glMemoryBarrier(barriers));
    }

    void DispatchCompute(GLuint num_groups_x, GLuint num_groups_y, GLuint num_groups_z SOURCE_LOCATION)
    {
        glCheck(glDispatchCompute(num_groups_x, num_groups_y, num_groups_z));
    }

    GLenum CheckNamedFramebufferStatus(GLuint framebuffer, GLenum target SOURCE_LOCATION)
    {
        glCheck(const GLenum status = glCheckNamedFramebufferStatus(framebuffer, target));
        return status;
    }

    void BindTextureUnit(GLuint unit, GLuint texture SOURCE_LOCATION)
    {
        glCheck(glBindTextureUnit(unit, texture));
    }

    void CreateBuffers(GLsizei n, GLuint* buffers SOURCE_LOCATION)
    {
        glCheck(glCreateBuffers(n, buffers));
    }

    void CreateFramebuffers(GLsizei n, GLuint* ids SOURCE_LOCATION)
    {
        glCheck(glCreateFramebuffers(n, ids));
    }

    void CreateTextures(GLenum target, GLsizei n, GLuint* textures SOURCE_LOCATION)
    {
        glCheck(glCreateTextures(target, n, textures));
    }

    void CreateVertexArrays(GLsizei n, GLuint* arrays SOURCE_LOCATION)
    {
        glCheck(glCreateVertexArrays(n, arrays));
    }

    void EnableVertexArrayAttrib(GLuint vaobj, GLuint index SOURCE_LOCATION)
    {
        glCheck(glEnableVertexArrayAttrib(vaobj, index));
    }

    void NamedBufferStorage(GLuint buffer, GLsizeiptr size, const void* data, GLbitfield flags SOURCE_LOCATION)
    {
        glCheck(glNamedBufferStorage(buffer, size, data, flags));
    }

    void NamedBufferSubData(GLuint buffer, GLintptr offset, GLsizei size, const void* data SOURCE_LOCATION)
    {
        glCheck(glNamedBufferSubData(buffer, offset, size, data));
    }

    void NamedFramebufferDrawBuffers(GLuint framebuffer, GLsizei n, const GLenum* bufs SOURCE_LOCATION)
    {
        glCheck(glNamedFramebufferDrawBuffers(framebuffer, n, bufs));
    }

    void NamedFramebufferTexture(GLuint framebuffer, GLenum attachment, GLuint texture, GLint level SOURCE_LOCATION)
    {
        glCheck(glNamedFramebufferTexture(framebuffer, attachment, texture, level));
    }

    void TextureParameterfv(GLuint texture, GLenum pname, const GLfloat* paramtexture SOURCE_LOCATION)
    {
        glCheck(glTextureParameterfv(texture, pname, paramtexture));
    }

    void TextureParameteri(GLuint texture, GLenum pname, GLint param SOURCE_LOCATION)
    {
        glCheck(glTextureParameteri(texture, pname, param));
    }

    void TextureStorage2D(GLuint texture, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height SOURCE_LOCATION)
    {
        glCheck(glTextureStorage2D(texture, levels, internalformat, width, height));
    }

    void TextureSubImage2D(GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void* pixels SOURCE_LOCATION)
    {
        glCheck(glTextureSubImage2D(texture, level, xoffset, yoffset, width, height, format, type, pixels));
    }

    void VertexArrayAttribBinding(GLuint vaobj, GLuint attribindex, GLuint bindingindex SOURCE_LOCATION)
    {
        glCheck(glVertexArrayAttribBinding(vaobj, attribindex, bindingindex));
    }

    void VertexArrayAttribFormat(GLuint vaobj, GLuint attribindex, GLint size, GLenum type, GLboolean normalized, GLuint relativeoffset SOURCE_LOCATION)
    {
        glCheck(glVertexArrayAttribFormat(vaobj, attribindex, size, type, normalized, relativeoffset));
    }

    void VertexArrayElementBuffer(GLuint vaobj, GLuint buffer SOURCE_LOCATION)
    {
        glCheck(glVertexArrayElementBuffer(vaobj, buffer));
    }

    void VertexArrayVertexBuffer(GLuint vaobj, GLuint bindingindex, GLuint buffer, GLintptr offset, GLsizei stride SOURCE_LOCATION)
    {
        glCheck(glVertexArrayVertexBuffer(vaobj, bindingindex, buffer, offset, stride));
    }

#endif

}
