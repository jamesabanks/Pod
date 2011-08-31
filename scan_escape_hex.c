int scan_escape_hex(pod_stream *stream, pod_char_t c)
{
    switch (c) {

        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            state = (stream->state & stream_state_mask) | stream_escape_hex;
            digit = (c - '0') & 0xf;
            number *= 0x10;
            number |= digit;
            break;
        case 'A':
        case 'B':
        case 'C':
        case 'D':
        case 'E':
        case 'F':
            state = (stream->state & stream_state_mask) | stream_escape_hex;
            digit = (10 + (c - 'A')) & 0xf
            number *= 0x10;
            number |= digit;
            break;
        case 'a':
        case 'b':
        case 'c':
        case 'd':
        case 'e':
        case 'f':
            state = (stream->state & stream_state_mask) | stream_escape_hex;
            digit = (10 + (c - 'a')) & 0xf;
            number *= 0x10;
            number |= digit;
            break;
        case '\\':
            break;
        default:
            syntax error

    }
