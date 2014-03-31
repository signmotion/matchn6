#include "../include/stdafx.h"
#include "../include/ManagerSprite.h"
#include "../include/ge/world/World.h"


namespace matchn {

    
ManagerSprite::ManagerSprite() :
    mKnownFrame()
{
}




ManagerSprite::~ManagerSprite() {
}




CL_Sprite*
ManagerSprite::sprite(
    CL_GraphicContext&    gc,
    const typelib::size2Int_t&  needVisualSize,
    const kindElement_t&  kind,
    const nameElement_t&  name,
    const nameState_t&    ns
) {
    const pathSprite_t ps = pathSprite( kind, name, ns );
    const auto sprite = loadSprite( gc, needVisualSize, ps );
    if ( sprite ) {
        return sprite;
    }

    // ������� ��� ����� ������� ���,
    // ��������� ������� ����� �� ���������
    const pathSprite_t psDef = pathSprite( kind, name, "default" );
    const auto spriteDef = loadSprite( gc, needVisualSize, psDef );
    DASSERT( spriteDef
        && "���������� ������ ��� �������� �� ������." );

    return spriteDef;
}




CL_PixelBuffer* ManagerSprite::frame( const std::string& path ) {

    ASSERT( !path.empty()
        && "���� � ������ �� ������." );

    // ��������� ���
    auto ftr = mKnownFrame.find( path );
    if (ftr != mKnownFrame.end()) {
        return ftr->second.get();
    }

    // � ���� ����� ����������� ���, ���������
#ifdef _DEBUG
    std::cout << "  ����� '" << path << "' .. ";
#endif
    const int posExtension = path.find_last_of( '.' );
    const std::string extension =
        (posExtension == std::string::npos)
            ? "" : path.substr( posExtension );
    const bool support =
        (extension == ".png")
     || (extension == ".jpg")
    ;
    if ( !support ) {
        ASSERT( false
            && "���� ��� ����������� �� ��������������." );
        return nullptr;
    }

    const auto image =
        (extension == ".png") ? CL_PNGProvider::load( path ) :
        (extension == ".jpg") ? CL_JPEGProvider::load( path ) :
        // #! ����� �������� ���� �� �������� ��� ClanLib-2.3.6 / Windows 7.
        CL_ImageProviderFactory::load( path );
#ifdef _DEBUG
    std::cout << " OK" << std::endl;
#endif

    // ��������
    std::unique_ptr< CL_PixelBuffer >
        imagePtr( new CL_PixelBuffer( image.copy() ) );
    ftr = mKnownFrame.insert(
        std::make_pair( path,  std::move( imagePtr ) )
    ).first;

    return ftr->second.get();
}




CL_Sprite*
ManagerSprite::loadSprite(
    CL_GraphicContext&  gc,
    const typelib::size2Int_t&  needVisualSize,
    const pathSprite_t&  path
) {
    // ������� �������� ������ �� ����
    const auto ftr = mKnownSprite.find( path );
    if (ftr != mKnownSprite.cend()) {
        return ftr->second.get();
    }


    // �������� ������ ��� �������
    // # ������ ������� ������������� �� ������ �������.
#ifdef _DEBUG
    std::cout << "\n�������������� ������ '" << path << "'\n";
#endif

    std::unique_ptr< CL_Sprite >  sprite;

    // ������ ��� ����������� ������
    CL_SpriteDescription ds;
    std::set< std::string >  frameSet;
    typelib::file::listFile( &frameSet, path, "png" );
    typelib::file::listFile( &frameSet, path, "jpg" );
    /* - # ���������, ��� ������� ��� ��������� ������� ���.
    ASSERT( !frameSet.empty()
        && "��� ������� �� ������� �� ������ ������. "
            " ���������� ����� �� ����� ���� ������." );
    */
    if ( !frameSet.empty() ) {
        for (auto ftr = frameSet.cbegin(); ftr != frameSet.cend(); ++ftr) {
            const std::string& f = *ftr;
            const std::string pathFrame = path + "/" + f;
            const auto image = frame( pathFrame );
            ds.add_frame( *image );
        }
        // ��� ���. �����
        sprite = std::unique_ptr< CL_Sprite >( new CL_Sprite( gc, ds ) );

        // @todo ������������� ������.

        // ������ ��������� ��� ������� - �� ������
        const int cx = sprite->get_width()  / 2;
        const int cy = sprite->get_height() / 2;
        sprite->set_alignment( origin_center );

        // ����� �������
        // # ��������� ������������ ������� �������.
        float fixSX, fixSY;
        sprite->get_scale( fixSX, fixSY );
        const typelib::size2_t  scale(
            needVisualSize.x / static_cast< float >( sprite->get_width()  ) * fixSX,
            needVisualSize.y / static_cast< float >( sprite->get_height() ) * fixSY
        );
        sprite->set_scale( scale.x, scale.y );

        sprite->update();

#ifdef _DEBUG
        std::cout << "������ '" << path << "' ������.\n";
#endif

    } else {
#ifdef _DEBUG
        std::cout << "������ '" << path << "' �� ������.\n";
#endif
    } // else if ( !frameSet.empty() )


    // # ���� ������ ��� ��� - ������� ���� ��������.
    const auto ltr = mKnownSprite.insert(
        std::make_pair( path, std::move( sprite ) )
    ).first;

    return ltr->second.get();
}


} // matchn
