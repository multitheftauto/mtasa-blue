import pytest

import build_gettext_catalog_nsi as nsi

NSI_SAMPLE = """
LangString	TEST_LABEL_1	${LANG_ENGLISH} "msgid"
LangString  test_label_2	${LANG_ENGLISH} "msgid2"

LangString  TEST_multiline  ${LANG_ENGLISH}   "msgid3\\n\\n\\
across\\n\\n\\
multiple\\n\\n\\
lines"

LangString  TEST_multiline_whitespace  ${LANG_ENGLISH}   "msgid3\\n\\n\\
    across\\n\\n\\
    multiple\\n\\n\\
    lines"
LangString  test_LABEL_4         ${LANG_ENGLISH} "msgid4"
LangString  test_blank  ${LANG_ENGLISH} ""
;LangString  test_commented  ${LANG_ENGLISH} "commented"
"""

def test_parse_nsi():
    # When
    lang_strings = nsi.parse_nsi(NSI_SAMPLE)
    # Then
    assert lang_strings == [
        nsi.LangString(nsi_label="TEST_LABEL_1", line_number=2, msgid="msgid"),
        nsi.LangString(nsi_label="test_label_2", line_number=3, msgid="msgid2"),
        nsi.LangString(
            nsi_label="TEST_multiline",
            line_number=5,
            msgid="msgid3\\n\\nacross\\n\\nmultiple\\n\\nlines",
        ),
        nsi.LangString(
            nsi_label="TEST_multiline_whitespace",
            line_number=10,
            msgid="msgid3\\n\\nacross\\n\\nmultiple\\n\\nlines",
        ),
        nsi.LangString(
            nsi_label="test_LABEL_4",
            line_number=14,
            msgid="msgid4",
        ),
        nsi.LangString(nsi_label="test_blank", line_number=15, msgid=""),

    ]



@pytest.mark.parametrize("input_pos, expected_line",[
    (0, 1),
    (3, 1),
    (4, 1),
    (5, 2),
    (6, 2),
    (9, 2),
    (10, 3),
    (16, 4),
    (27, 5),
])
def test_get_line_number(input_pos, expected_line):
    # Given
    sample_text = "aaaa\naaaa\naaaa\naaaa\naaaa"
    # When
    positions = nsi.get_newline_positions(sample_text)
    line_no = nsi.get_line_number_from_position(positions, input_pos)
    # Then
    assert line_no == expected_line
