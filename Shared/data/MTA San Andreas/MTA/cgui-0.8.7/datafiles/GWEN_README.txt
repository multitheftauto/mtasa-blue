GWEN Skin README
------------------

This skin is based on the default skin for the GWEN GUI System(http://code.google.com/p/gwen/).
Graphics were taken from GWEN which is under the MIT License, which you can read here,
http://www.opensource.org/licenses/mit-license.php or at the bottom of this document.

This skin was ported by Chris Johnson. Contact me at chris@cjohnson.id.au if you have any
questions. If you have any support requests please visit the CEGUI forums.

This skin is also under the MIT License. You can read it in license.txt

There are several differences between the CEGUI and GWEN skin due to the differences between
these two systems. Some differences/issues are listed below:

 - Combo Edit Box/EditBox
    - Caret Color in Text Boxes made pink because CEGUI won't make it blink
    - Selected Text Box state not ported
    - Read Only edit boxes have pink outline. This is usually used for hover part in GWEN.
 - Combo Edit Box
    - Menu Box seems to leave white faded border on left
    - Didn't use shadow on drop down
 - Group Box
    - Groupbox in CEGUI only supports images for the borders. Therefore I've displayed the
      label at the top. Alternatively could display white or another color behind the text
    - ComboDropLists and Listboxes uses GWEN disabled color
 - Image Button
    - Set the disabled image button to be displayed as normal because otherwise it is not
      shown
 - Listbox
    - Items don't have odd and even coloring or correct highlighting on mouse over
 - Menu
    - No shadow around popup
    - Forced padding on Menu Items as Font Horizontal Extend was too short
 - Multicolumn List
    - Whitespace appears on the far right of the list headers
    - Should add mouse cursor when resizing columns
 - Progress Bar
    - Can only support 1px wide border for progress bar
    - Disabled Progress bar is not supported in GWEN. Therefore adopted greyed out approach.
 - Tab Control
    - Couldn't replicate GWEN tab system exactly.

---------------------------------------

The MIT License (MIT)
Copyright (c) <2011> <Garry Newman>

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
