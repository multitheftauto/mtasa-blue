/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        Shared/core/CFileSystem.Utils.hxx
*  PURPOSE:     FileSystem common utilities
*  DEVELOPERS:  The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef _FILESYSUTILS_
#define _FILESYSUTILS_

static inline bool _File_ParseRelativePath( const char *path, dirTree& tree, bool& file )
{
    filePath entry;
    entry.reserve( MAX_PATH );

    while (*path)
    {
        switch (*path)
        {
        case '\\':
        case '/':
            if ( entry.empty() )
                break;

            if ( entry == "." )
            {
                // We ignore this current path indicator
                entry.clear();
                break;
            }
            else if ( entry == ".." )
            {
                if ( tree.empty() )
                    return false;

                tree.pop_back();
                entry.clear();
                break;
            }

            tree.push_back( entry );
            entry.clear();
            break;
        case ':':
            return false;
#ifdef _WIN32
        case '<':
        case '>':
        case '"':
        case '|':
        case '?':
        case '*':
            return false;
#endif
        default:
            entry += *path;
            break;
        }

        path++;
    }

    if ( !entry.empty() )
    {
        tree.push_back( entry );

        file = true;
    }
    else
        file = false;

    return true;
}

// Output a path tree
static inline void _File_OutputPathTree( const dirTree& tree, bool file, filePath& output )
{
    unsigned int n;

    if ( file )
    {
        if ( tree.empty() )
            return;

        for (n=0; n<tree.size()-1; n++)
        {
            output += tree[n];
            output += '/';
        }

        output += tree[n];
        return;
    }

    for (n=0; n<tree.size(); n++)
    {
        output += tree[n];
        output += '/';
    }
}

// Get relative path tree nodes
static inline bool _File_ParseRelativeTree( const char *path, const dirTree& root, dirTree& output, bool& file )
{
    dirTree tree;
    dirTree::const_iterator rootIter;
    dirTree::iterator treeIter;

    if (!_File_ParseRelativePath( path, tree, file ))
        return false;

    if ( file )
    {
        if ( tree.size() <= root.size() )
            return false;
    }
    else if ( tree.size() < root.size() )
        return false;

    for ( rootIter = root.begin(), treeIter = tree.begin(); rootIter != root.end(); rootIter++, treeIter++ )
        if ( *rootIter != *treeIter )
            return false;

    output.insert( output.end(), treeIter, tree.end() );
    return true;
}

static const filePath _dirBack = "..";

static inline bool _File_ParseDeriviation( const dirTree& curTree, dirTree::const_iterator& treeIter, dirTree& output, size_t sizeDiff, bool file )
{
    for ( dirTree::const_iterator rootIter( curTree.begin() ); rootIter != curTree.end(); rootIter++, treeIter++, sizeDiff-- )
    {
        if ( !sizeDiff )
        {
            // This cannot fail, as we go down to root at maximum
            for ( ; rootIter != curTree.end(); rootIter++ )
                output.push_back( _dirBack );

            if ( file )
                output.push_back( *treeIter );

            return true;
        }

        if ( *rootIter != *treeIter )
        {
            for ( ; rootIter != curTree.end(); rootIter++ )
                output.push_back( _dirBack );

            break;
        }
    }

    return false;
}

static inline bool _File_ParseDeriviateTreeRoot( const dirTree& pathFind, const dirTree& curTree, dirTree& output, bool file )
{
    dirTree::const_iterator treeIter( pathFind.begin() );

    if ( _File_ParseDeriviation( curTree, treeIter, output, file ? pathFind.size() - 1 : pathFind.size(), file ) )
        return true;

    for ( ; treeIter != pathFind.end(); treeIter++ )
        output.push_back( *treeIter );

    return true;
}

static inline bool _File_ParseDeriviateTree( const char *path, const dirTree& curTree, dirTree& output, bool& file )
{
    dirTree tree = curTree;

    if ( !_File_ParseRelativePath( path, tree, file ) )
        return false;

    return _File_ParseDeriviateTreeRoot( tree, curTree, output, file );
}

// Do the same as above, but derive from current directory
static inline bool _File_ParseRelativeTreeDeriviate( const char *path, const dirTree& root, const dirTree& curTree, dirTree& output, bool& file )
{
    dirTree tree;
    dirTree::const_iterator rootIter;
    dirTree::iterator treeIter;
    size_t sizeDiff;

    if (!_File_ParseRelativePath( path, tree, file ))
        return false;

    if ( file )
    {
        // File should not count as directory
        if ( tree.size() <= root.size() )
            return false;

        sizeDiff = tree.size() - root.size() - 1;
    }
    else
    {
        if ( tree.size() < root.size() )
            return false;

        sizeDiff = tree.size() - root.size();
    }

    for ( rootIter = root.begin(), treeIter = tree.begin(); rootIter != root.end(); rootIter++, treeIter++ )
        if ( *rootIter != *treeIter )
            return false;

    if ( _File_ParseDeriviation( curTree, treeIter, output, sizeDiff, file ) )
        return true;

    output.insert( output.end(), treeIter, tree.end() );
    return true;
}

inline bool _File_ParseMode( const CFileTranslator& root, const filePath& path, const char *mode, unsigned int& access, unsigned int& m )
{
    switch( *mode )
    {
    case 'w':
        m = FILE_MODE_CREATE;
        access = FILE_ACCESS_WRITE;
        break;
    case 'r':
        m = FILE_MODE_OPEN;
        access = FILE_ACCESS_READ;
        break;
    case 'a':
        m = root.Exists( path ) ? FILE_MODE_OPEN : FILE_MODE_CREATE;
        access = FILE_ACCESS_WRITE;
        break;
#ifdef _EXTRA_FILE_ACCESS_MODES
    case 't':
        break;
#endif
    default:
        return false;
    }

    if ( mode[1] == 'b' && mode[2] == '+' || mode[1] == '+' )
        access |= FILE_ACCESS_WRITE | FILE_ACCESS_READ;

    return true;
}

#endif //_FILESYSUTILS_