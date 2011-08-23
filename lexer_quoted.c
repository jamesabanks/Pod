
quoted_maxtrix
{
    {   '\\',   0,              lexer_quoted_escape }
    {   '"',    0,              lexer_initial }
    {   '\n',   0,              lexer_initial }
    {   '\r',   0               lexer_initial }
    {   other,  0,              lexer_quoted }
    { other<32, 0,              lexer_quoted, warn }
}

(remember, state is quoted)
find character in list
if char is other
    add to string
state = new_state
