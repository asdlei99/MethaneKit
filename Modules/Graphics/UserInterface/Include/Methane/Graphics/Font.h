/******************************************************************************

Copyright 2020 Evgeny Gorodetskiy

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

*******************************************************************************

FILE: Methane/Graphics/Font.h
Font atlas textures generation and fonts library management classes.

******************************************************************************/

#pragma once

#include <Methane/Graphics/RenderContext.h>
#include <Methane/Graphics/Texture.h>
#include <Methane/Graphics/Types.h>
#include <Methane/Data/Provider.h>
#include <Methane/Data/Emitter.hpp>

#include <map>
#include <string>
#include <cctype>

namespace Methane::Graphics
{

class Font;

struct IFontCallback
{
    virtual void OnFontAtlasTextureReset(Font& font, const Ptr<Texture>& sp_old_atlas_texture, const Ptr<Texture>& sp_new_atlas_texture) = 0;
    virtual void OnFontAtlasUpdated(Font& font) = 0;

    virtual ~IFontCallback() = default;
};

class Font
    : public std::enable_shared_from_this<Font>
    , public Data::Emitter<IFontCallback>
    , protected Data::Receiver<IContextCallback>
{
public:
    struct Settings
    {
        std::string name;
        std::string font_path;
        uint32_t    font_size_pt;
        uint32_t    resolution_dpi;
        std::string characters;
    };

    class Library
    {
        friend class Font;

    public:
        static Library& Get();

        Refs<Font> GetFonts() const;
        bool  HasFont(const std::string& font_name) const;
        Font& GetFont(const std::string& font_name) const;
        Font& GetFont(const Data::Provider& data_provider, const Settings& font_settings);
        Font& AddFont(const Data::Provider& data_provider, const Settings& font_settings);
        void  RemoveFont(const std::string& font_name);
        void  Clear();

    protected:
        class Impl;
        Impl& GetImpl() { return *m_sp_impl; }

    private:
        Library();

        using FontByName = std::map<std::string, Ptr<Font>>;

        const UniquePtr<Impl> m_sp_impl;
        FontByName            m_font_by_name;
    };

    class CharBinPack;

    class Char
    {
        friend class CharBinPack;

    public:
        class Glyph;
        using Code = uint32_t;

        struct Type
        {
            using Mask = uint8_t;
            enum Value : Mask
            {
                Unknown    = 0u,
                Whitespace = 1u << 0u,
                LineBreak  = 1u << 1u,
            };

            static Mask Get(Code code);

            Type() = delete;
        };

        Char() = default;
        Char(Code code);
        Char(Code code, FrameRect rect, Point2i offset, Point2i advance, UniquePtr<Glyph>&& sp_glyph);

        Code             GetCode() const noexcept        { return m_code; }
        bool             IsLineBreak() const noexcept    { return m_type_mask & Type::LineBreak; }
        bool             IsWhiteSpace() const noexcept   { return m_type_mask & Type::Whitespace; }
        const FrameRect& GetRect() const noexcept        { return m_rect; }
        const Point2i&   GetOffset() const noexcept      { return m_offset; }
        const Point2i&   GetAdvance() const noexcept     { return m_advance; }

        bool operator<(const Char& other) const noexcept { return m_rect.size.GetPixelsCount() < other.m_rect.size.GetPixelsCount(); }
        bool operator>(const Char& other) const noexcept { return m_rect.size.GetPixelsCount() > other.m_rect.size.GetPixelsCount(); }
        operator bool() const noexcept                   { return m_code != 0u; }

        void     DrawToAtlas(Data::Bytes& atlas_bitmap, uint32_t atlas_row_stride) const;
        uint32_t GetGlyphIndex() const;

    private:
        const Code       m_code = 0u;
        const Type::Mask m_type_mask = Type::Value::Unknown;
        FrameRect        m_rect;
        Point2i          m_offset;
        Point2i          m_advance;
        UniquePtr<Glyph> m_sp_glyph;
    };

    using Chars = Refs<const Char>;

    static std::string GetAnsiCharacters(char from = 32, char to = 126);
    static std::string GetTextAlphabet(const std::string& text);

    ~Font();

    Ptr<Font>       GetPtr() { return shared_from_this(); }
    const Settings& GetSettings() const { return m_settings; }

    void              ResetChars(const std::string& unicode_characters);
    void              ResetChars(const std::wstring& characters);
    void              AddChars(const std::string& unicode_characters);
    void              AddChars(const std::wstring& characters);
    const Font::Char& AddChar(Char::Code char_code);
    bool              HasChar(Char::Code char_code);
    const Char&       GetChar(Char::Code char_code) const;
    Chars             GetChars() const;
    Chars             GetTextChars(const std::string& text);
    FrameRect::Point  GetKerning(const Char& left_char, const Char& right_char) const;
    const FrameSize&  GetMaxGlyphSize() const { return m_max_glyph_size; }

    const Ptr<Texture>& GetAtlasTexturePtr(Context& context);
    Texture&            GetAtlasTexture(Context& context) { return *GetAtlasTexturePtr(context); }

protected:
    // Font can be created only via Font::Library::Add
    Font(const Data::Provider& data_provider, const Settings& settings);

    Refs<Char> GetMutableChars();
    bool PackCharsToAtlas(float pixels_reserve_multiplier);

    // IContextCallback interface
    void OnContextReleased(Context& context) override;
    void OnContextInitialized(Context&) override { }

private:
    Ptr<Texture> CreateAtlasTexture(Context& context);
    void RemoveAtlasTexture(Context& context);

    bool UpdateAtlasBitmap();
    void UpdateAtlasTextures();
    void ClearAtlasTextures();

    class Face;
    using TextureByContext = std::map<Context*, Ptr<Texture>>;
    using CharByCode = std::map<Char::Code, Char>;

    Settings               m_settings;
    UniquePtr<Face>        m_sp_face;
    UniquePtr<CharBinPack> m_sp_atlas_pack;
    CharByCode             m_char_by_code;
    Data::Bytes            m_atlas_bitmap;
    TextureByContext       m_atlas_textures;
    FrameSize              m_max_glyph_size;
};

} // namespace Methane::Graphics
