
string_matrix
{
    {   '{',    begin_list,     lexer_initial }
    {   '}',    end_list,       lexer_initial }
    {   '\\',   0,              lexer_string_escape }
    {   '<',    begin_map,      lexer_initial }
    {   '>',    end_map,        lexer_initial }
    {   '[',    begin_blurb,    lexer_blurb }
    {   ']',    end_blurb,      lexer_initial, error }
    {   '"',    0,              lexer_quoted }
    {   '=',    equals,         lexer_initial }
    {   '+',    0,              lexer_initial }
    {   ' ',    0,              lexer_initial }
    {   '\t',   0,              lexer_initial }
    {   '\n',   0,              lexer_initial }
    {   '\r',   0,              lexer_initial }
    {   other,  0,              lexer_string }
    { other<32, 0,              lexer_string, warn }
}

(remember, state is string)
find character in list
if char is '+', set have_concat to true
else if char sends token
    set have_concat to false
    send(string)
    send(token)
if char is '"'
    set have_concat to false
    send(string)
if char is other
    add to string
state = new_state
