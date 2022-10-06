#include <fcntl.h>
#include <memory.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#define int int64_t

/**
 * @brief pointers && registers && counters
 *
 */
int MAX_SIZE = 1024 * 1024;
char *data, *src, *src_dump;
int *pc, *sp, *bp, *code, *code_dump, *stack, *symbol_table, *symbol_pointer, *main_pointer;
int ax, cycle, token, token_val, line, type, new_bp;

/**
 * @brief instructions
 *
 */
enum
{
    IMM,
    LEA,
    JMP,
    JZ,
    JNZ,
    CALL,
    NVAR,
    DARG,
    RET,
    LI,
    LC,
    SI,
    SC,
    PUSH,
    OR,
    XOR,
    AND,
    EQ,
    NE,
    LT,
    GT,
    LE,
    GE,
    SHL,
    SHR,
    ADD,
    SUB,
    MUL,
    DIV,
    MOD,
    OPEN,
    READ,
    CLOS,
    PRTF,
    MALC,
    FREE,
    MSET,
    MCMP,
    EXIT
};

/**
 * @brief classes && tokens
 *
 */
enum
{
    Num = 128,
    Fun,
    Sys,
    Glo,
    Loc,
    Id,
    Char,
    Int,
    Enum,
    If,
    Else,
    Return,
    Sizeof,
    While,
    Assign,
    Cond,
    Lor,
    Land,
    Or,
    Xor,
    And,
    Eq,
    Ne,
    Lt,
    Gt,
    Le,
    Ge,
    Shl,
    Shr,
    Add,
    Sub,
    Mul,
    Div,
    Mod,
    Inc,
    Dec,
    Brak
};

/**
 * @brief identifer fields
 *
 */
enum
{
    Token,
    Hash,
    Name,
    Class,
    Type,
    Value,
    GClass,
    GType,
    GValue,
    SymSize
};

/**
 * @brief types
 *
 */
enum
{
    CHAR,
    INT,
    PTR
};


/**
 * @brief replacing sensitive data with unique identification symbols
 * while retain all the essential information.
 *
 */
void tokenizer()
{
    char *ch_ptr;
    while ((token = *src++))
    {
        if (token == '\n')
            line++;
        else if (token == '#')
            while (*src != 0 && *src != '\n')
                src++;
        else if ((token >= 'a' && token <= 'z') || (token >= 'A' && token <= 'Z') || (token == '_'))
        {
            ch_ptr = src - 1;
            while ((*src >= 'a' && *src <= 'z') || (*src >= 'A' && *src <= 'Z') || (*src >= '0' && *src <= '9') || (*src == '_'))
                token = token * 147 + *src++;
            token = (token << 6) + (src - ch_ptr);
            symbol_pointer = symbol_table;
            while (symbol_pointer[Token])
            {
                if (token == symbol_pointer[Hash] && !memcmp((char *)symbol_pointer[Name], ch_ptr, src - ch_ptr))
                {
                    token = symbol_pointer[Token];
                    return;
                }
                symbol_pointer = symbol_pointer + SymSize;
            }
            symbol_pointer[Name] = (int)ch_ptr;
            symbol_pointer[Hash] = token;
            token = symbol_pointer[Token] = Id;
            return;
        }
        else if (token >= '0' && token <= '9')
        {
            if ((token_val = token - '0'))
                while (*src >= '0' && *src <= '9')
                    token_val = token_val * 10 + *src++ - '0';
            else if (*src == 'x' || *src == 'X')
                while ((token = *++src) && ((token >= '0' && token <= '9') || (token >= 'a' && token <= 'f') || (token >= 'A' && token <= 'F')))
                    token_val = token_val * 16 + (token & 0xF) + (token >= 'A' ? 9 : 0);
            else
                while (*src >= '0' && *src <= '7')
                    token_val = token_val * 8 + *src++ - '0';
            token = Num;
            return;
        }
        else if (token == '"' || token == '\'')
        {
            ch_ptr = data;
            while (*src != 0 && *src != token)
            {
                if ((token_val = *src++) == '\\')
                {
                    if ((token_val = *src++) == 'n')
                        token_val = '\n';
                }
                if (token == '"')
                    *data++ = token_val;
            }
            src++;
            if (token == '"')
                token_val = (int)ch_ptr;
            else
                token = Num;
            return;
        }
        else if (token == '/')
        {
            if (*src == '/')
            {
                while (*src != 0 && *src != '\n')
                    src++;
            }
            else
            {
                token = Div;
                return;
            }
        }
        else if (token == '=')
        {
            if (*src == '=')
            {
                src++;
                token = Eq;
            }
            else
                token = Assign;
            return;
        }
        else if (token == '+')
        {
            if (*src == '+')
            {
                src++;
                token = Inc;
            }
            else
                token = Add;
            return;
        }
        else if (token == '-')
        {
            if (*src == '-')
            {
                src++;
                token = Dec;
            }
            else
                token = Sub;
            return;
        }
        else if (token == '!')
        {
            if (*src == '=')
            {
                src++;
                token = Ne;
            }
            return;
        }
        else if (token == '<')
        {
            if (*src == '=')
            {
                src++;
                token = Le;
            }
            else if (*src == '<')
            {
                src++;
                token = Shl;
            }
            else
                token = Lt;
            return;
        }
        else if (token == '>')
        {
            if (*src == '=')
            {
                src++;
                token = Ge;
            }
            else if (*src == '>')
            {
                src++;
                token = Shr;
            }
            else
                token = Gt;
            return;
        }
        else if (token == '|')
        {
            if (*src == '|')
            {
                src++;
                token = Lor;
            }
            else
                token = Or;
            return;
        }
        else if (token == '&')
        {
            if (*src == '&')
            {
                src++;
                token = Land;
            }
            else
                token = And;
            return;
        }
        else if (token == '^')
        {
            token = Xor;
            return;
        }
        else if (token == '%')
        {
            token = Mod;
            return;
        }
        else if (token == '*')
        {
            token = Mul;
            return;
        }
        else if (token == '[')
        {
            token = Brak;
            return;
        }
        else if (token == '?')
        {
            token = Cond;
            return;
        }
        else if (token == '~' || token == ';' || token == '{' || token == '}' || token == '(' || token == ')' || token == ']' || token == ',' || token == ':')
            return;
    }
}

/**
 * @brief Report an assert error
 *
 * @param tk
 */
void macro_assert(int tk)
{
    if (token != tk)
    {
        printf("%lld SyntaxError: invalid token.\n", line);
        exit(-1);
    }
    tokenizer();
}

/**
 * @brief identity verification
 *
 */
void identity_verification()
{
    if (token != Id)
    {
        printf("%lld: SyntaxError: invalid identifer.\n", line);
        exit(-1);
    }
    if (symbol_pointer[Class] == Loc)
    {
        printf("%lld: SyntaxError: duplicate declaration.\n", line);
        exit(-1);
    }
}

/**
 * @brief remove the global property
 *
 */
void remove_global_property()
{
    symbol_pointer[GClass] = symbol_pointer[Class];
    symbol_pointer[GType] = symbol_pointer[Type];
    symbol_pointer[GValue] = symbol_pointer[Value];
}

/**
 * @brief recover the global property
 *
 */
void recover_global_property()
{
    symbol_pointer[Class] = symbol_pointer[GClass];
    symbol_pointer[Type] = symbol_pointer[GType];
    symbol_pointer[Value] = symbol_pointer[GValue];
}

/**
 * @brief enumerated constants parser
 *
 */
void enumerated_constants_parser()
{
    int i;
    i = 0;
    while (token != '}')
    {
        identity_verification();
        macro_assert(Id);
        if (token == Assign)
        {
            macro_assert(Assign);
            macro_assert(Num);
            i = token_val;
        }
        symbol_pointer[Class] = Num;
        symbol_pointer[Type] = INT;
        symbol_pointer[Value] = i++;
        if (token == ',')
            tokenizer();
    }
}

/**
 * @brief type parser
 *
 * @return int
 */
int type_parser()
{
    if (token == Char)
    {
        macro_assert(Char);
        return CHAR;
    }
    else
    {
        macro_assert(Int);
        return INT;
    }
}

/**
 * @brief parameter parser
 *
 */
void parameter_parser()
{
    int type, i;
    i = 0;
    while (token != ')')
    {
        type = type_parser();
        while (token == Mul)
        {
            macro_assert(Mul);
            type = type + PTR;
        }
        identity_verification();
        macro_assert(Id);
        remove_global_property();
        symbol_pointer[Class] = Loc;
        symbol_pointer[Type] = type;
        symbol_pointer[Value] = i++;
        if (token == ',')
            macro_assert(',');
    }
    new_bp = ++i;
}

/**
 * @brief expression parser
 *
 * @param precd
 */
void expression_parser(int precd)
{
    int tmp_type, i;
    int *tmp_ptr;
    if (token == Num)
    {
        tokenizer();
        *++code = IMM;
        *++code = token_val;
        type = INT;
    }
    else if (token == '"')
    {
        *++code = IMM;
        *++code = token_val;
        macro_assert('"');
        while (token == '"')
            macro_assert('"');
        data = (char *)((int)data + 8 & -8);
        type = PTR;
    }
    else if (token == Sizeof)
    {
        tokenizer();
        macro_assert('(');
        type = type_parser();
        while (token == Mul)
        {
            macro_assert(Mul);
            type = type + PTR;
        }
        macro_assert(')');
        *++code = IMM;
        *++code = (type == CHAR) ? 1 : 8;
        type = INT;
    }
    else if (token == Id)
    {
        tokenizer();
        tmp_ptr = symbol_pointer;
        if (token == '(')
        {
            macro_assert('(');
            i = 0;
            while (token != ')')
            {
                expression_parser(Assign);
                *++code = PUSH;
                i++;
                if (token == ',')
                    macro_assert(',');
            }
            macro_assert(')');
            if (tmp_ptr[Class] == Sys)
                *++code = tmp_ptr[Value];
            else if (tmp_ptr[Class] == Fun)
            {
                *++code = CALL;
                *++code = tmp_ptr[Value];
            }
            else
            {
                printf("%lld: LinkerError: invalid function call.\n", line);
                exit(-1);
            }
            if (i > 0)
            {
                *++code = DARG;
                *++code = i;
            }
            type = tmp_ptr[Type];
        }
        else if (tmp_ptr[Class] == Num)
        {
            *++code = IMM;
            *++code = tmp_ptr[Value];
            type = INT;
        }
        else
        {
            if (tmp_ptr[Class] == Loc)
            {
                *++code = LEA;
                *++code = new_bp - tmp_ptr[Value];
            }
            else if (tmp_ptr[Class] == Glo)
            {
                *++code = IMM;
                *++code = tmp_ptr[Value];
            }
            else
            {
                printf("%lld: SyntaxError: invalid variable.\n", line);
                exit(-1);
            }
            type = tmp_ptr[Type];
            *++code = (type == CHAR) ? LC : LI;
        }
    }
    else if (token == '(')
    {
        macro_assert('(');
        if (token == Char || token == Int)
        {
            tokenizer();
            tmp_type = token - Char + CHAR;
            while (token == Mul)
            {
                macro_assert(Mul);
                tmp_type = tmp_type + PTR;
            }
            macro_assert(')');
            expression_parser(Inc);
            type = tmp_type;
        }
        else
        {
            expression_parser(Assign);
            macro_assert(')');
        }
    }
    else if (token == Mul)
    {
        tokenizer();
        expression_parser(Inc);
        if (type >= PTR)
            type = type - PTR;
        else
        {
            printf("%lld: LinkerError: invalid dereference.\n", line);
            exit(-1);
        }
        *++code = (type == CHAR) ? LC : LI;
    }
    else if (token == And)
    {
        tokenizer();
        expression_parser(Inc);
        if (*code == LC || *code == LI)
            code--;
        else
        {
            printf("%lld: LinkerError: invalid reference.\n", line);
            exit(-1);
        }
        type = type + PTR;
    }
    else if (token == '!')
    {
        tokenizer();
        expression_parser(Inc);
        *++code = PUSH;
        *++code = IMM;
        *++code = 0;
        *++code = EQ;
        type = INT;
    }
    else if (token == '~')
    {
        tokenizer();
        expression_parser(Inc);
        *++code = PUSH;
        *++code = IMM;
        *++code = -1;
        *++code = XOR;
        type = INT;
    }
    else if (token == And)
    {
        tokenizer();
        expression_parser(Inc);
        type = INT;
    }
    else if (token == Sub)
    {
        tokenizer();
        expression_parser(Inc);
        *++code = PUSH;
        *++code = IMM;
        *++code = -1;
        *++code = MUL;
        type = INT;
    }
    else if (token == Inc || token == Dec)
    {
        i = token;
        tokenizer();
        expression_parser(Inc);
        if (*code == LC)
        {
            *code = PUSH;
            *++code = LC;
        }
        else if (*code == LI)
        {
            *code = PUSH;
            *++code = LI;
        }
        else
        {
            printf("%lld: SemanticError: invalid Inc or Dec.\n", line);
            exit(-1);
        }
        *++code = PUSH;
        *++code = IMM;
        *++code = (type > PTR) ? 8 : 1;
        *++code = (i == Inc) ? ADD : SUB;
        *++code = (type == CHAR) ? SC : SI;
    }
    else
    {
        printf("%lld: SyntaxError: invalid expression.\n", line);
        exit(-1);
    }
    while (token >= precd)
    {
        tmp_type = type;
        if (token == Assign)
        {
            tokenizer();
            if (*code == LC || *code == LI)
                *code = PUSH;
            else
            {
                printf("%lld: SemanticError: invalid assignment.\n", line);
                exit(-1);
            }
            expression_parser(Assign);
            type = tmp_type;
            *++code = (type == CHAR) ? SC : SI;
        }
        else if (token == Cond)
        {
            tokenizer();
            *++code = JZ;
            tmp_ptr = ++code;
            expression_parser(Assign);
            macro_assert(':');
            *tmp_ptr = (int)(code + 3);
            *++code = JMP;
            tmp_ptr = ++code;
            expression_parser(Cond);
            *tmp_ptr = (int)(code + 1);
        }
        else if (token == Lor)
        {
            tokenizer();
            *++code = JNZ;
            tmp_ptr = ++code;
            expression_parser(Land);
            *tmp_ptr = (int)(code + 1);
            type = INT;
        }
        else if (token == Land)
        {
            tokenizer();
            *++code = JZ;
            tmp_ptr = ++code;
            expression_parser(Or);
            *tmp_ptr = (int)(code + 1);
            type = INT;
        }
        else if (token == Or)
        {
            tokenizer();
            *++code = PUSH;
            expression_parser(Xor);
            *++code = OR;
            type = INT;
        }
        else if (token == Xor)
        {
            tokenizer();
            *++code = PUSH;
            expression_parser(And);
            *++code = XOR;
            type = INT;
        }
        else if (token == And)
        {
            tokenizer();
            *++code = PUSH;
            expression_parser(Eq);
            *++code = AND;
            type = INT;
        }
        else if (token == Eq)
        {
            tokenizer();
            *++code = PUSH;
            expression_parser(Lt);
            *++code = EQ;
            type = INT;
        }
        else if (token == Ne)
        {
            tokenizer();
            *++code = PUSH;
            expression_parser(Lt);
            *++code = NE;
            type = INT;
        }
        else if (token == Lt)
        {
            tokenizer();
            *++code = PUSH;
            expression_parser(Shl);
            *++code = LT;
            type = INT;
        }
        else if (token == Gt)
        {
            tokenizer();
            *++code = PUSH;
            expression_parser(Shl);
            *++code = GT;
            type = INT;
        }
        else if (token == Le)
        {
            tokenizer();
            *++code = PUSH;
            expression_parser(Shl);
            *++code = LE;
            type = INT;
        }
        else if (token == Ge)
        {
            tokenizer();
            *++code = PUSH;
            expression_parser(Shl);
            *++code = GE;
            type = INT;
        }
        else if (token == Shl)
        {
            tokenizer();
            *++code = PUSH;
            expression_parser(Add);
            *++code = SHL;
            type = INT;
        }
        else if (token == Shr)
        {
            tokenizer();
            *++code = PUSH;
            expression_parser(Add);
            *++code = SHR;
            type = INT;
        }
        else if (token == Add)
        {
            tokenizer();
            *++code = PUSH;
            expression_parser(Mul);
            if (tmp_type > PTR)
            {
                *++code = PUSH;
                *++code = IMM;
                *++code = 8;
                *++code = MUL;
            }
            *++code = ADD;
            type = tmp_type;
        }
        else if (token == Sub)
        {
            tokenizer();
            *++code = PUSH;
            expression_parser(Mul);
            if (tmp_type > PTR && tmp_type == type)
            {
                *++code = SUB;
                *++code = PUSH;
                *++code = IMM;
                *++code = 8;
                *++code = DIV;
                type = INT;
            }
            else if (tmp_type > PTR)
            {
                *++code = PUSH;
                *++code = IMM;
                *++code = 8;
                *++code = MUL;
                *++code = SUB;
                type = tmp_type;
            }
            else
                *++code = SUB;
        }
        else if (token == Mul)
        {
            tokenizer();
            *++code = PUSH;
            expression_parser(Inc);
            *++code = MUL;
            type = INT;
        }
        else if (token == Div)
        {
            tokenizer();
            *++code = PUSH;
            expression_parser(Inc);
            *++code = DIV;
            type = INT;
        }
        else if (token == Mod)
        {
            tokenizer();
            *++code = PUSH;
            expression_parser(Inc);
            *++code = MOD;
            type = INT;
        }
        else if (token == Inc || token == Dec)
        {
            if (*code == LC)
            {
                *code = PUSH;
                *++code = LC;
            }
            else if (*code == LI)
            {
                *code = PUSH;
                *++code = LI;
            }
            else
            {
                printf("%lld: invlid operator=%lld\n", line, token);
                exit(-1);
            }
            *++code = PUSH;
            *++code = IMM;
            *++code = (type > PTR) ? 8 : 1;
            *++code = (token == Inc) ? ADD : SUB;
            *++code = (type == CHAR) ? SC : SI;
            *++code = PUSH;
            *++code = IMM;
            *++code = (type > PTR) ? 8 : 1;
            *++code = (token == Inc) ? SUB : ADD;
            tokenizer();
        }
        else if (token == Brak)
        {
            macro_assert(Brak);
            *++code = PUSH;
            expression_parser(Assign);
            macro_assert(']');
            if (tmp_type > PTR)
            {
                *++code = PUSH;
                *++code = IMM;
                *++code = 8;
                *++code = MUL;
            }
            else if (tmp_type < PTR)
            {
                printf("%lld: SyntaxError: invalid index op. \n", line);
                exit(-1);
            }
            *++code = ADD;
            type = tmp_type - PTR;
            *++code = (type == CHAR) ? LC : LI;
        }
        else
        {
            printf("%lld: invlid token=%lld\n", line, token);
            exit(-1);
        }
    }
}

/**
 * @brief statement parser
 *
 */
void statement_parser()
{
    int *a;
    int *b;
    if (token == If)
    {
        macro_assert(If);
        macro_assert('(');
        expression_parser(Assign);
        macro_assert(')');
        *++code = JZ;
        b = ++code;
        statement_parser();
        if (token == Else)
        {
            macro_assert(Else);
            *b = (int)(code + 3);
            *++code = JMP;
            b = ++code;
            statement_parser();
        }
        *b = (int)(code + 1);
    }
    else if (token == While)
    {
        macro_assert(While);
        a = code + 1;
        macro_assert('(');
        expression_parser(Assign);
        macro_assert(')');
        *++code = JZ;
        b = ++code;
        statement_parser();
        *++code = JMP;
        *++code = (int)a;
        *b = (int)(code + 1);
    }
    else if (token == Return)
    {
        macro_assert(Return);
        if (token != ';')
            expression_parser(Assign);
        macro_assert(';');
        *++code = RET;
    }
    else if (token == '{')
    {
        macro_assert('{');
        while (token != '}')
            statement_parser(Assign);
        macro_assert('}');
    }
    else if (token == ';')
        macro_assert(';');
    else
    {
        expression_parser(Assign);
        macro_assert(';');
    }
}

/**
 * @brief function parser
 *
 */
void function_parser()
{
    int type, i;
    i = new_bp;
    while (token == Char || token == Int)
    {
        type = type_parser();
        while (token != ';')
        {
            while (token == Mul)
            {
                macro_assert(Mul);
                type = type + PTR;
            }
            identity_verification();
            macro_assert(Id);
            remove_global_property();
            symbol_pointer[Class] = Loc;
            symbol_pointer[Type] = type;
            symbol_pointer[Value] = ++i;
            if (token == ',')
                macro_assert(',');
        }
        macro_assert(';');
    }
    *++code = NVAR;
    *++code = i - new_bp;
    while (token != '}')
        statement_parser();
    if (*code != RET)
        *++code = RET;
    symbol_pointer = symbol_table;
    while (symbol_pointer[Token])
    {
        if (symbol_pointer[Class] == Loc)
            recover_global_property();
        symbol_pointer = symbol_pointer + SymSize;
    }
}

/**
 * @brief takes input data and builds a data structure
 *
 */
void parser()
{
    int type, base_type;
    line = 1;
    token = 1;
    while (token > 0)
    {
        tokenizer();
        if (token == Enum)
        {
            macro_assert(Enum);
            if (token != '{')
                macro_assert(Id);
            macro_assert('{');
            enumerated_constants_parser();
            macro_assert('}');
        }
        else if (token == Int || token == Char)
        {
            base_type = type_parser();
            while (token != ';' && token != '}')
            {
                type = base_type;
                while (token == Mul)
                {
                    macro_assert(Mul);
                    type = type + PTR;
                }
                identity_verification();
                macro_assert(Id);
                symbol_pointer[Type] = type;
                if (token == '(')
                {
                    symbol_pointer[Class] = Fun;
                    symbol_pointer[Value] = (int)(code + 1);
                    macro_assert('(');
                    parameter_parser();
                    macro_assert(')');
                    macro_assert('{');
                    function_parser();
                }
                else
                {
                    symbol_pointer[Class] = Glo;
                    symbol_pointer[Value] = (int)data;
                    data = data + 8;
                }
                if (token == ',')
                    macro_assert(',');
            }
        }
    }
}

/**
 * @brief index term
 *
 */
void index_term()
{
    int i;
    src = "char int enum if else return sizeof while "
          "open read close printf malloc free memset memcmp exit void main";
    i = Char;
    while (i <= While)
    {
        tokenizer();
        symbol_pointer[Token] = i++;
    }
    i = OPEN;
    while (i <= EXIT)
    {
        tokenizer();
        symbol_pointer[Class] = Sys;
        symbol_pointer[Type] = INT;
        symbol_pointer[Value] = i++;
    }
    tokenizer();
    symbol_pointer[Token] = Char;
    tokenizer();
    main_pointer = symbol_pointer;
    src = src_dump;
}

/**
 * @brief initialize theVirtual Machine
 *
 * @return int
 */
int initialize_Virtual_Machine()
{
    if (!(code = code_dump = malloc(MAX_SIZE)))
    {
        printf("malloc(%lld) failed: code segment\n", MAX_SIZE);
        return -1;
    }
    if (!(data = malloc(MAX_SIZE)))
    {
        printf("malloc(%lld) failed: data segment\n", MAX_SIZE);
        return -1;
    }
    if (!(stack = malloc(MAX_SIZE)))
    {
        printf("malloc(%lld) failed: stack segment\n", MAX_SIZE);
        return -1;
    }
    if (!(symbol_table = malloc(MAX_SIZE / 16)))
    {
        printf("malloc(%lld) failed: symbol_table\n", MAX_SIZE / 16);
        return -1;
    }
    memset(code, 0, MAX_SIZE);
    memset(data, 0, MAX_SIZE);
    memset(stack, 0, MAX_SIZE);
    memset(symbol_table, 0, MAX_SIZE / 16);
    return 0;
}

/**
 * @brief run Virtual Machine
 *
 * @param argc
 * @param argv
 * @return int
 */
int start_Virtual_Machine(int argc, char **argv)
{
    int op;
    int *tmp;
    bp = sp = (int *)((int)stack + MAX_SIZE);
    *--sp = EXIT;
    *--sp = PUSH;
    tmp = sp;
    *--sp = argc;
    *--sp = (int)argv;
    *--sp = (int)tmp;
    if (!(pc = (int *)main_pointer[Value]))
    {
        printf("function not found: main function for architecture x86_64\n");
        exit(-1);
    }
    cycle = 0;
    while (1)
    {
        cycle++;
        op = *pc++;
        if (op == IMM)
            ax = *pc++;
        else if (op == LEA)
            ax = (int)(bp + *pc++);
        else if (op == LC)
            ax = *(char *)ax;
        else if (op == LI)
            ax = *(int *)ax;
        else if (op == SC)
            *(char *)*sp++ = ax;
        else if (op == SI)
            *(int *)*sp++ = ax;
        else if (op == PUSH)
            *--sp = ax;
        else if (op == JMP)
            pc = (int *)*pc;
        else if (op == JZ)
            pc = ax ? pc + 1 : (int *)*pc;
        else if (op == JNZ)
            pc = ax ? (int *)*pc : pc + 1;
        else if (op == OR)
            ax = *sp++ | ax;
        else if (op == XOR)
            ax = *sp++ ^ ax;
        else if (op == AND)
            ax = *sp++ & ax;
        else if (op == EQ)
            ax = *sp++ == ax;
        else if (op == NE)
            ax = *sp++ != ax;
        else if (op == LT)
            ax = *sp++ < ax;
        else if (op == LE)
            ax = *sp++ <= ax;
        else if (op == GT)
            ax = *sp++ > ax;
        else if (op == GE)
            ax = *sp++ >= ax;
        else if (op == SHL)
            ax = *sp++ << ax;
        else if (op == SHR)
            ax = *sp++ >> ax;
        else if (op == ADD)
            ax = *sp++ + ax;
        else if (op == SUB)
            ax = *sp++ - ax;
        else if (op == MUL)
            ax = *sp++ * ax;
        else if (op == DIV)
            ax = *sp++ / ax;
        else if (op == MOD)
            ax = *sp++ % ax;
        else if (op == CALL)
        {
            *--sp = (int)(pc + 1);
            pc = (int *)*pc;
        }
        else if (op == NVAR)
        {
            *--sp = (int)bp;
            bp = sp;
            sp = sp - *pc++;
        }
        else if (op == DARG)
            sp = sp + *pc++;
        else if (op == RET)
        {
            sp = bp;
            bp = (int *)*sp++;
            pc = (int *)*sp++;
        }
        else if (op == OPEN)
        {
            ax = open((char *)sp[1], sp[0]);
        }
        else if (op == CLOS)
        {
            ax = close(*sp);
        }
        else if (op == READ)
        {
            ax = read(sp[2], (char *)sp[1], *sp);
        }
        else if (op == PRTF)
        {
            tmp = sp + pc[1] - 1;
            ax = printf((char *)tmp[0], tmp[-1], tmp[-2], tmp[-3], tmp[-4], tmp[-5]);
        }
        else if (op == MALC)
        {
            ax = (int)malloc(*sp);
        }
        else if (op == FREE)
        {
            free((void *)*sp);
        }
        else if (op == MSET)
        {
            ax = (int)memset((char *)sp[2], sp[1], *sp);
        }
        else if (op == MCMP)
        {
            ax = memcmp((char *)sp[2], (char *)sp[1], *sp);
        }
        else if (op == EXIT)
        {
            return *sp;
        }
        else
        {
            printf("instruction not found: %lld, cycle: %lld\n", op, cycle);
            return -1;
        }
    }
    return 0;
}

/**
 * @brief Low level functions for loading data
 *
 * @param file
 * @return int
 */
int load_src(char *file)
{
    int fd, cnt;
    if ((fd = open(file, 0)) < 0)
    {
        printf("could not open: source code(%s)\n", file);
        return -1;
    }
    if (!(src = src_dump = malloc(MAX_SIZE)))
    {
        printf("malloc(%lld) failed: source code\n", MAX_SIZE);
        return -1;
    }
    if ((cnt = read(fd, src, MAX_SIZE - 1)) <= 0)
    {
        printf("could not read: source code(%lld)\n", cnt);
        return -1;
    }
    src[cnt] = 0;
    close(fd);
    return 0;
}

/**
 * @brief print the logo at beginning of execution
 *
 * @return int
 */
int print_logo()
{
    printf("                                ▄   ▄\n██████╗ ███████╗ ██████╗██████╗ █▀█▀█\n██╔══██╗██╔════╝██╔════╝██╔══██╗█▄█▄█\n██████╔╝███████╗██║     ██████╔╝███  ▄▄\n██╔══██╗╚════██║██║     ██╔═══╝ ████▐█ █\n██████╔╝███████║╚██████╗██║     ████   █\n╚═════╝ ╚══════╝ ╚═════╝╚═╝     ▀▀▀▀▀▀▀\n");
    return 0;
}

/**
 * @brief main function for BSCP
 *
 * @param argc
 * @param argv
 * @return int32_t
 */
int32_t main(int32_t argc, char **argv)
{
    print_logo();
    if (load_src(*(argv + 1)) != 0)
        return -1;
    if (initialize_Virtual_Machine() != 0)
        return -1;
    index_term();
    parser();
    return start_Virtual_Machine(--argc, ++argv);
}
