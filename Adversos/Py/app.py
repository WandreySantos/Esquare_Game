from PIL import Image

# Exemplo de execução no Git Bash:
# Priemiro abra a pasta Py no Git Bash

# site pra criar a  img:
#         https://www.piskelapp.com/p/create/sprite
# salve a img como PNG com 16x16 px

# Depois rode o comando: 
#         python app.py

# salve a img dentri da pasta que estao o App.py

from PIL import Image

# Define cores que você quer transformar em macros automaticamente
def rgb_to_macro_name(r, g, b):
    # Básicas
    if (r, g, b) == (0, 0, 0): return "PRETO"
    if (r, g, b) == (255, 255, 255): return "BRANCO"

    # Verdes
    if (r, g, b) == (168, 230, 29): return "VERDE"
    if (r, g, b) == (0, 255, 0): return "VERDE_FORTE"
    if (r, g, b) == (100, 200, 50): return "VERDE_MEIO"
    if (r, g, b) == (30, 120, 30): return "VERDE_ESCURO"

    # Vermelhos
    if (r, g, b) == (237, 28, 36): return "VERMELHO"
    if (r, g, b) == (255, 60, 60): return "VERMELHO_CLARO"
    if (r, g, b) == (150, 20, 20): return "VERMELHO_ESCURO"

    # Amarelos
    if (r, g, b) == (255, 242, 0): return "AMARELO"
    if (r, g, b) == (255, 200, 0): return "AMARELO_FORTE"
    if (r, g, b) == (200, 170, 0): return "AMARELO_QUEIMADO"

    # Azuis
    if (r, g, b) == (77, 109, 243): return "AZUL"
    if (r, g, b) == (0, 120, 255): return "AZUL_FORTE"
    if (r, g, b) == (0, 70, 180): return "AZUL_MEDIO"
    if (r, g, b) == (0, 30, 100): return "AZUL_ESCURO"

    # Roxos / Lilases
    if (r, g, b) == (180, 0, 255): return "ROXO"
    if (r, g, b) == (140, 0, 200): return "ROXO_ESCURO"
    if (r, g, b) == (200, 120, 255): return "LILAS"

    # Laranjas
    if (r, g, b) == (255, 140, 0): return "LARANJA"
    if (r, g, b) == (255, 100, 0): return "LARANJA_FORTE"
    if (r, g, b) == (200, 80, 0): return "LARANJA_ESCURO"

    # Rosa
    if (r, g, b) == (255, 105, 180): return "ROSA"
    if (r, g, b) == (255, 60, 150): return "ROSA_FORTE"
    if (r, g, b) == (200, 40, 120): return "ROSA_ESCURO"

    # Se a cor não estiver na lista
    return f"CRGB({r},{g},{b})"


def img_to_crgb_macros(image_path):
    """
    Converte uma imagem para um array CRGB 16x16 usando macros, pronto para Arduino.
    """
    img = Image.open(image_path).convert('RGB')
    img = img.resize((16, 16), resample=Image.NEAREST)

    # Descobre todas cores usadas
    cores_usadas = set(img.getpixel((x, y)) for y in range(16) for x in range(16))
    # Cria macros
    print("// --- Definição das cores ---")
    for cor in cores_usadas:
        nome = rgb_to_macro_name(*cor)
        if "CRGB" not in nome:
            print(f"#define {nome} CRGB({cor[0]},{cor[1]},{cor[2]})")
    print("\n// --- Matriz 16x16 ---")
    print("const CRGB image16x16[16*16] = {")

    for y in range(16):
        linha = []
        for x in range(16):
            r, g, b = img.getpixel((x, y))
            linha.append(rgb_to_macro_name(r, g, b))
        print("  " + ", ".join(linha) + ",")
    print("};\n")
    print("✅ Copie o array para seu código Arduino!")

# --- Uso ---
if __name__ == "__main__":
    caminho = input("Digite o caminho da imagem (ex: desenho.png): ")
    img_to_crgb_macros(caminho)

