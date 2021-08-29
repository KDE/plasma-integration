/*
    SPDX-FileCopyrightText: 2020 Carson Black <uhhadd@gmail.com>

    SPDX-License-Identifier: LGPL-2.0-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include <QList>
#include <QMap>
#include <QString>

// Curating key data
// =================
//
// There are two main goals with key data here:
// - to allow users to input letters related to but not in their keyboard language (e.g. Persian glyphs on top of the Arabic set)
// - to allow users to input useful symbols, such as proper arithmetic symbols and various dashes
//
// The former generally is used for letters, while the latter is generally used for punctuation and numbers
//
// Latin Letters:
//
// Generally, these are going to be diacritics (a + ` = à) or diagraphs (a + e = æ).
// Letters should be roughly ordered first by amount of speakers of languages that use them,
// and then by commonality in languages as a secondary factor. For example, the character å is
// found mostly in northern Germanic languages, which don't have nearly as many speakers as
// romance languages such as Spanish, which has á. For this reason, á should be ordered before å.
// Rinse and repeat this process until all the variants you want to offer are well placed.
//
// Cyrilic Letters:
//
// The same principles as Latin letters apply here. For example, the character ҝ (Ka with a vertical stroke; Azerbaijani)
// is found in a much less common language than қ (Ka with a descender; many languages found in former Soviet Union territories)
// so it should be placed after қ in the list of options for к (Ka; lot of languages)
//
// Arabic characters:
//
// Same deal as Latin and Cyrilic, except now it's going to render poorly in your text editors.
// Most of these characters only have one alternate form, mostly to add some extra diacritics
// in order to use a less common form or a form not found in Arabic, e.g. Persian پ from Arabic ب.
// Alef is a special case here, as it has a lot of "variants" in common use. For example, there's the
// alef with a hamza sitting on it (ا + ʾ = أ) or a hamza sitting below it (إ). These can be inputted
// as diagraphs on keyboards, but can also be represented as a held-key variant of alef for input purposes.
//
// Hebrew characters:
//
// The characters here are the iffiest of the selection, since most don't add anything new to input, just a
// "convenience" way for inputting characters with a ׳ added, when you can already type it without extra modifiers.
// However, we're still having them there since they're a logical "variant" of the characters they're for.
//
// On the other hand, the ײַ ײ ױ װ work as held keys to offer a meaningful alternative to other methods of inputting
// them, since these typically require the usage of AltGr to input on most Hebrew keyboards.
//
// Numbers:
//
// These are mostly exponents and fractions. Their ordering should be self explanatory.
//
// Symbols:
//
// These are mostly things that look like the key being held, with the exception
// of ^ being used for directional arrows.
//

namespace KeyData
{
const QMap<QString, QList<QString>> KeyMappings = {
    //
    // Latin
    //
    {"a", {"à", "á", "â", "ä", "æ", "ã", "å", "ā"}},
    {"c", {"ç", "ć", "č"}},
    {"d", {"ð"}},
    {"e", {"è", "é", "ê", "ë", "ē", "ė", "ę", "ə"}},
    {"g", {"ğ"}},
    {"i", {"î", "ï", "í", "ī", "į", "ì", "ı"}},
    {"l", {"ł"}},
    {"n", {"ñ", "ń"}},
    {"o", {"ô", "ö", "ò", "ó", "œ", "ø", "ō", "õ"}},
    {"s", {"ß", "ś", "š", "ş"}},
    {"u", {"û", "ü", "ù", "ú", "ū"}},
    {"x", {"×"}},
    {"y", {"ÿ", "ұ", "ү", "ӯ", "ў"}},
    {"z", {"ž", "ź", "ż"}},
    //
    // Cyrilic
    //
    {"г", {"ғ"}},
    {"е", {"ё"}}, // this in fact NOT the same E as before
    {"и", {"ӣ", "і"}}, // і is not i
    {"й", {"ј"}}, // ј is not j
    {"к",
     {
         "қ",
         "ҝ",
     }},
    {"н", {"ң", "һ"}}, // һ is not h
    {"о", {"ә", "ө"}},
    {"ч", {"ҷ", "ҹ"}},
    {"ь", {"ъ"}},
    //
    // Arabic
    //
    // This renders weirdly in text editors, but is valid code.
    {"ا", {"أ", "إ", "آ", "ء"}},
    {"ب", {"پ"}},
    {"ج", {"چ"}},
    {"ز", {"ژ"}},
    {"ف", {"ڤ"}},
    {"ك", {"گ"}},
    {"ل", {"لا"}},
    {"ه", {"ه"}},
    {"و", {"ؤ"}},
    //
    // Hebrew
    //
    // Likewise, this will render oddly, but is still valid code.
    {"ג", {"ג׳"}},
    {"ז", {"ז׳"}},
    {"ח", {"ח׳"}},
    {"צ׳", {"צ׳"}},
    {"ת", {"ת׳"}},
    {"י", {"ײַ"}},
    {"י", {"ײ"}},
    {"ח", {"ױ"}},
    {"ו", {"װ"}},
    //
    // Numbers
    //
    {"0", {"∅", "ⁿ", "⁰"}},
    {"1", {"¹", "½", "⅓", "¼", "⅕", "⅙", "⅐", "⅛", "⅑", "⅒"}},
    {"2", {"²", "⅖", "⅔"}},
    {"3", {"³", "⅗", "¾", "⅜"}},
    {"4", {"⁴", "⅘", "⁵", "⅝", "⅚"}},
    {"5", {"⁵", "⅝", "⅚"}},
    {"6", {"⁶"}},
    {"7", {"⁷", "⅞"}},
    {"8", {"⁸"}},
    {"9", {"⁹"}},
    //
    // Punctuation
    //
    {R"(-)", {"—", "–", "·"}},
    {R"(?)", {"¿", "‽"}},
    {R"(')", {"‘", "’", "‚", "‹", "›"}},
    {R"(!)", {"¡"}},
    {R"(")", {"“", "”", "„", "«", "»"}},
    {R"(/)", {"÷"}},
    {R"(#)", {"№"}},
    {R"(%)", {"‰", "℅"}},
    {R"(^)", {"↑", "←", "→", "↓"}},
    {R"(+)", {"±"}},
    {R"(<)", {"«", "≤", "‹", "⟨"}},
    {R"(=)", {"∞", "≠", "≈"}},
    {R"(>)", {"⟩", "»", "≥", "›"}},
    //
    // Currency
    //
    {"$", {"¢", "€", "£", "¥", "₹", "₽", "₺", "₩", "₱", "₿"}},
};

}
