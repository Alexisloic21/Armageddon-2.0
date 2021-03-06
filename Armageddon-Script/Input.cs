
using System.Runtime.CompilerServices;

namespace Armageddon
{
    public enum KeyCode
    {
        backspace = 8,
        tab = 9,
        enter = 13,
        shift = 16,
        ctrl = 17,
        alt = 18,
        pause = 19,
        caps_lock = 20,
        escape = 27,
        page_up = 33,
        Space = 32,
        page_down = 34,
        end = 35,
        home = 36,
        arrow_left = 37,
        arrow_up = 38,
        arrow_right = 39,
        arrow_down = 40,
        print_screen = 44,
        insert = 45,
        delete = 46,
        letter_0 = 48,
        letter_1 = 49,
        letter_2 = 50,
        letter_3 = 51,
        letter_4 = 52,
        letter_5 = 53,
        letter_6 = 54,
        letter_7 = 55,
        letter_8 = 56,
        letter_9 = 57,
        A = 65,
        B = 66,
        C = 67,
        D = 68,
        E = 69,
        F = 70,
        G = 71,
        H = 72,
        I = 73,
        J = 74,
        K = 75,
        L = 76,
        M = 77,
        N = 78,
        O = 79,
        P = 80,
        Q = 81,
        R = 82,
        S = 83,
        T = 84,
        U = 85,
        V = 86,
        W = 87,
        X = 88,
        Y = 89,
        Z = 90,
        WINDOWS1 = 91,
        WINDOWS = 92,
        select = 93,
        numpad_0 = 96,
        numpad_1 = 97,
        numpad_2 = 98,
        numpad_3 = 99,
        numpad_4 = 100,
        numpad_5 = 101,
        numpad_6 = 102,
        numpad_7 = 103,
        numpad_8 = 104,
        numpad_9 = 105,
        multiply = 106,
        add = 107,
        subtract = 109,
        decimal_point = 110,
        divide = 111,
        f1 = 112,
        f2 = 113,
        f3 = 114,
        f4 = 115,
        f5 = 116,
        f6 = 117,
        f7 = 118,
        f8 = 119,
        f9 = 120,
        f10 = 121,
        f11 = 122,
        f12 = 123,
        num_lock = 144,
        scroll_lock = 145,
        My = 182 /*Computer(multimedia keyboard)*/,
        My2 = 183 /*Calculator(multimedia keyboard)*/,
        semicolon = 186,
        equalsign = 187,
        comma = 188,
        dash = 189,
        period = 190,
        forward_slash = 191,
        open_bracket = 219,
        back_slash = 220,
        close_braket = 221,
        single_quote = 222
    }
   public class Input
    {
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern bool IsKeyPressed(KeyCode code);
        public static bool GetKey(string key)
        {
            KeyCode code = (KeyCode)System.Enum.Parse(typeof(KeyCode), key);
            return IsKeyPressed(code);
        }
    }
}
