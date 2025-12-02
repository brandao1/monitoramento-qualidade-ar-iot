import sys
import re
import os

if len(sys.argv) != 3:
    print("Usage: python convert_porter_to_header.py rf_model_porter.c rf_model.h")
    sys.exit(1)

src = sys.argv[1]
dst = sys.argv[2]
text = open(src, "r").read()

text = text.replace("double", "float")

text = text.replace("sqrt(", "sqrtf(")
text = text.replace("fabs(", "fabsf(")
text = text.replace("pow(", "powf(")

text = re.sub(r'\bint\s+predict\s*\(', 'int predict_rf(', text)

text = re.sub(r'#include\s+<stdio.h>.*\n', '', text)
text = re.sub(r'#include\s+<stdlib.h>.*\n', '', text)

header_guard = os.path.basename(dst).upper().replace('.', '_')
wrapped = []
wrapped.append(f"#ifndef {header_guard}")
wrapped.append(f"#define {header_guard}")
wrapped.append("")
wrapped.append("#include <math.h>")
wrapped.append("")
wrapped.append("// Predict function generated from sklearn-porter and adapted for ESP32")
wrapped.append(text)
wrapped.append("")
wrapped.append("// Helper wrapper: expõe predict_rf(const float *x) retornando 0/1 as classes\n")
wrapped.append("#endif // " + header_guard + "\n")

open(dst, "w").write("\n".join(wrapped))
print("Header gerado em", dst)
print("Inspecione o arquivo e verifique a função predict_rf para confirmar a assinatura e as classes retornadas.")