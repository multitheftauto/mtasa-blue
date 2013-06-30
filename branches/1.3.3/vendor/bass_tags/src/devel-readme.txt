
TAGS - Yet Another Tags Reading Library

Written by Wraith, 2k5-2k6
Delphi header and sample by Chris Troesken

Public domain. No warranty.

Version 8

Developer README file.

0. Overview.

I've decided to include source code this time - because I'm not going to make money or whatever off this library. Those who want to extend it, or learn from it (whatever), or just curious about how it's written - enjoy.

The library is written in C++. It was developed using VC7.1 then ported to VC6.0, because 6.0 produces better library in terms of OS compatibility. There is some compiler-specific code (like casting char* to non-POD pointers (unspecified behavior)  and #pragmas). The code will compile correctly on ICC, though, because it's bug-to-bug compatible with Mircrosoft's. And of course, the code is Windows-specific.

1. Getting started.

Copy the latest bass.h, bass.lib to basslib/. Optionally, you may copy add-on headers bass_ape.h, bassflac.h, bass_ofr.h basswma.h to basslib/.
Copy bass.dll to bin/.

Run IDE, find libtags project and edit config.h.
Remove Bass add=on header you do not wish to use.

2. Build the solution/workspace.

3. Perform tests.

Get an mp3 with ID3v1, ID3v2, both tags and copy it to bin/. Go ahead, and use your favorite tagger. Also look for some ogg, wma and ape tracks and put them into bin/ to. Now rename them according to what is named in tags-test/test.cpp and run the test. Now make some damage to the tags. Use a flawed tagger, or just write "Hello, world!" right into the ID3v2 tag overwriting some frames. See if it produces some "undesired behaviour" with the dll (eg. a crash).

4. Some inside information, or How This Damn Thing Works.

The library consists of three major components: format string parser (keyword.h/cpp), tag selector (tags_impl.h,rselect.h/cpp) and a set of tag readers (id3v1.cpp, id3v2.cpp, ogg.cpp, etc).

When TAGS_Read(); is called, the first thing it does is trying to see which tag type the stream has. It calls into tag selector (rselect.cpp), which in turn, obtains stream ctype from Bass. Tag guessing mechanism using a table of ctypes looks for a corresponding tag type (or just tries every known tag type until BASS_StreamGetTags() returns something) and calls an appropriate reader generator (i.e. a function that creates a tag reader instance). All tag readers have the same interface, as could be seen at tags_impl.h. The actual tag reader implementations differ.

The tag selector returns created tag reader object back to TAGS_Read(). The first part is done.
TAGS_Read() calls the format string parser and gives it the tag reader object (keywords.h/cpp). The parser looks through the text string and when it encounters one of %-identifiers (eg. %ALBM, %ARTI, etc.), it calls into the tag reader passing this identifier, and the tag reader returns a text string with what's the tag contains. 

When parser encounters an identifier that is a function, like %IFV, it tries to acquire a C++ function associated with the keyword, and then calls the keyword's function. The parser itself is recursive, so the %-function calls can be nested.

Each tag reader contains a 'remap', i.e. a table that maps common %-identifiers like %ARTI into tag-specific information, e.g corresponding field offset and length in ID3v1, corresponding frame name in ID3v2, etc. Also ID3v1 and v2 have parsers for various tag fields, because some fields (eg. genre in ID3v2) require processing according to standards. 

Each tag reader is independent from the others.

4.1. Tag reader interface specification:

A pointer to tag reader object should be compatible with pointer to this structure:

struct tag_reader
{
	virtual std::string operator[] ( const char* name ) const = 0;
	virtual ~tag_reader() {}
};

(Eg. tag reader class may be derived from this structure)

std::string operator[]( const char* name ) const;
The method accepts a %-identifier, an asciiz string in ANSI encoding and returns the text string from the corresponding tag field. The following %-identifiers are currently supported:

 "%TITL"  - song title;
 "%ARTI"  - song artist;
 "%ALBM"  - album name;
 "%GNRE"  - song genre;
 "%YEAR"  - song/album year;
 "%CMNT"  - comment;
 "%TRCK"  - track number;

If tag reader encounters an unknown %-identifer, it should throw struct bad_ident{} exception. The current implementation of text parser simply catches the exception and continues execution. This is intended to separate an 'invalid argument' (i.e. an external error) from tag inconsitencies (i.e. internal error), see below.

If tag reader is unable to locate the required infirmation in the tag, or it cannot correctly interpret the contained information it should return an empty string. It should not throw in this case.

No other exceptions are permitted, because the library is intended for non-C++ audience and C++ exceptions should not penetrate through module bounds. Although I could write an exception specification
for the function, I find it useless, because they are not supported by the compiler.

Destructor should not throw any exceptions.

A tag reader should be accompanied with an appropriate generator function, having the
following signature (as listed in rselect.h):

typedef tag_reader* (generator_signature)( DWORD );

The function accepts a single DWORD argument - a Bass channel handle, and returns a pointer to tag reader object.

It may not return NULL.

The function should throw no_tag exception if this particular tag reader cannot read tag data from the channel. (Eg. this happens when stream has BASS_TAG_OGG, but an ID3v1 tag reader is created. The reader tries to read BASS_TAG_ID3V1 using BASS_StreamGetTags(), gets NULL and throws no_tag). This exception indicates that this particular tag reader is unable to read the tag, but nothing critical happens and tag guessing mechanism goes on looking for an appropriate tag reader.

The function should throw bad_tag exception if this particular tag reader matches the stream tag type, but the tag format itself is invalid, eg. ID3v1 header is wrong. This indicates more severe error and the tag guessing mechanism stops, returns zero, so TAGS_Read() returns an error, because it doesn't get a tag reader object.

Again, no other exceptions are permitted.

5. Extending library functionality.

The whole library was designed with scalability in mind, so it's very easy to expand the functionality without breaking too much code.

To add a new tag reader (suppose you want mp4 tags), simply implement it as a class derived from tag_reader. Then implement corresponding tag reader generator function, that takes one argument - stream handle and returns pointer to tag reader object. Add generator's prototype to rselect.h, and add generator to the table in rselect.cpp.

To add a new tag field retrieving capability simply add new identifiers into corresponding tag reader %-identifier table. Eg. you can add support for "Lyricist/Text writer" field in ID3v2.3 by simply adding 
{ "%LYRI", field("TEXT", &text ) } 
to the remap_data[] in the id3v2.cpp.

To add a new %-function, simply add its definition to keywords.cpp, then add pointer to it to known_keywords[]. Look at existing %-function to get an idea.

5. Some notes on compiling/linking.

When I was preparing the very first version back in 2005 I came up to a strange dilemma: which runtime library should I use? If I choose 'Multithreaded DLL' in VC7.1, the dll will requre msvcr71.dll and msvcp71.dll to be present on target machine. Those come with Windows XP, but not with Windows 98. In fact, Win98 has only msvcrt.dll. If I choose Single-treaded or Multithreaded (i.e. static libc(mt).lib and libcp(mt).lib ), the DLL size goes up and weighs more than bass.dll itself!

So I ported the project to VC6.0, then made up an interesting hack: I specified that I want to use Multithreaded runtime library (static), which effectively links against libcmt.lib and libcpmt.lib. In the linker section I wrote to ignore libcmt.lib and link against msvcrt.lib instead. That is, I link dynamically against C-runtime, but statically against C++-runtime. The resulting dll depends only on msvcrt.dll. Then I just minimized C++-runtime usage (i.e. dropped iostreams, boost and some string-related stuff, also dropped off std::map and replaced it with simplemap, the only thing I couldn't drop was std::string), all of which results in 44k dll, and when packed using UPX, shrinks to 15k. This DLL goes into the distribution.

The DLL can be easily converted into a static library. This, however, leads to several issues. First, it limits the library use to C/C++-only environment (no Delphi or VB). Second (unconfirmed), this most likely won't work with non-MS compilers (I remember Ian asking me to use /link50compat when building the dll, and it had something to do with MiniGW). Third, you have to match the version of runtime library used to compile the library and that used to compile you application it's used in.

There are two .def-files: for debug and release versions. The only difference is the library name. Linkers seem to have trouble recognizing that debug version is created under 'tags_d.dll' name (/OUT:../bin/tags_d.dll) and still produces 'tags.lib' in debug build (which is the name for release version).

Known issues

 - Unicode support is somewhat crufty IMO.
 - Parser does not print a diagnostic if it finds an unmatched ')'. It simply stops parsing and returns successfully.
 - Upper-case conversion works only for ANSI characters. It doesn't work for letters from national alphabet in your OEM encoding.
 - ID3v2.3 tag reader silently strips 0xD symbols from comments, regardless of the TAGS_ID3V2_FACIST switch.
 - ID3v2.3 tag reader does not support compressed or encrypted frames.
 