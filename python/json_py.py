# TODO (shouldn't be hard :-)):
# - Implement _json_parse_string() (Teach you UTF-8)
# - Handle trailing comma in _json_parse_array() and _json_parse_object()
# - Consistent error handling, throw an exception or return a value?

class _Lexer:
    def __init__(self):
        self.input = None
        self.line = 1
        self.cur = 0
        self.iseof = False

    def __repr__(self):
        return "<EOF>" if self.iseof else self.input[self.cur]

    def __eq__(self, what):
        return False if self.iseof else self.input[self.cur] == what

    def set_input(self, inp):
        self.input = inp

    def isws(self):
        char = ord(self.input[self.cur])
        return char == 0x9 or char == 0xa or char == 0xd or char == 0x20

    def isdigit(self):
        return False if self.iseof else self.input[self.cur].isdigit()

    def lower(self):
        if self.iseof:
            raise ValueError("No more input to consume")
        return self.input[self.cur].lower()

    def skip_ws(self):
        if self.iseof:
           return 
        while self.cur < len(self.input) and self.isws():
            self.cur += 1
        if self.cur >= len(self.input):
            self.iseof = True

    def skip_input(self, n):
        self.cur += n
        if self.cur >= len(self.input):
            self.iseof = True

    def next_token(self):
        self.skip_ws()
        if self.cur < len(self.input):
            self.cur += 1
        else:
            self.iseof = True

    def subcmp(self, what):
        start = self.cur
        end = start + len(what)
        return self.input[start:end] == what

class json_py:
    def __init__(self):
        self._lex = _Lexer()

    def parse(self, inp=None):
        if type(inp) != str:
            raise ValueError("Input must be a string")
        elif len(inp) == 0:
            raise ValueError("Empty input")
        self._lex.set_input(inp)
        self._lex.skip_ws()
        if self._lex != '{' and self._lex != '[':
            val = self._json_parse_value()
            self._lex.skip_ws()
            if not self._lex.iseof:
                raise ValueError("Trailing character in single JSON value")
            return val
        return self._json_parse_value()

    def _json_parse_object(self):
        if self._lex != '{':
            raise ValueError(f"'{{' expected but found {self._lex} instead")

        self._lex.next_token()
        obj = dict()

        if self._lex == '}':
            return obj

        while True:
            key = self._json_parse_string()
            if self._lex != ':':
                raise ValueError(f"':' expected but found {self._lex} instead")
            self._lex.next_token()
            val = self._json_parse_value()
            obj[key] = val
            if self._lex != ',':
                break
            self._lex.next_token()

        if self._lex != '}':
            raise ValueError("Missing '}' while parsing JSON object")

        self._lex.next_token()
        return obj

    def _json_parse_array(self):
        if self._lex != '[':
            raise ValueError(f"'[' expected but found {self._lex} instead")

        self._lex.next_token()
        arr = list()

        if self._lex == ']':
            return arr

        while True:
            val = self._json_parse_value()
            arr.append(val)
            if self._lex != ',':
                break
            self._lex.next_token()
        
        if self._lex != ']':
            raise ValueError("Missing ']' while parsing JSON array")
        
        self._lex.next_token()
        return arr

    def _json_parse_number(self):
        start = self._lex.cur
        is_float = False

        if self._lex == '-':
            self._lex.next_token()

        if self._lex == '0':
            self._lex.next_token()
            if self._lex.isdigit():
                raise ValueError("Leading zero is not allowed in JSON")
        elif self._lex.isdigit():
            while self._lex.isdigit():
                self._lex.next_token()
        else:
            raise ValueError("Invalid input found while parsing JSON number")

        if self._lex == '.':
            is_float = True
            self._lex.next_token()
            if not self._lex.isdigit():
                raise ValueError("No digit found after found fractional sign")
            while self._lex.isdigit():
                self._lex.next_token()

        if self._lex.lower() == 'e':
            is_float = True
            self._lex.next_token()
            if self._lex == '-' or self._lex == '+':
                self._lex.next_token()
            if not self._lex.isdigit():
                raise ValueError("No digit found after found exponent sign")
            while self._lex.isdigit():
                self._lex.next_token()

        end = self._lex.cur
        conv = float if is_float else int
        self._lex.skip_ws()
        return conv(self._lex.input[start:end])

    def _json_parse_value(self):
        self._lex.skip_ws()

        if self._lex == '{':
            return self._json_parse_object()
        elif self._lex == '[':
            return self._json_parse_array()
        elif self._lex == '"':
            return self._json_parse_string()
        elif self._lex == '-' or self._lex.isdigit():
            return self._json_parse_number()
        else:
            if self._lex.subcmp("false"):
                self._lex.skip_input(5)
                self._lex.skip_ws()
                return False
            elif self._lex.subcmp("null"):
                self._lex.skip_input(4)
                self._lex.skip_ws()
                return None
            elif self._lex.subcmp("true"):
                self._lex.skip_input(4)
                self._lex.skip_ws()
                return True
            raise ValueError("Invalid JSON value")
