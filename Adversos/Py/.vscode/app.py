from PIL import Image

# Exemplo de execução no Git Bash:
# "/c/Users/wandr/AppData/Local/Programs/Python/Python313/python.exe" "./.vscode/app.py"

from PIL import Image

# Define cores que você quer transformar em macros automaticamente
def rgb_to_macro_name(r, g, b):
    # Cria nomes simples baseados na cor
    if (r, g, b) == (0, 0, 0): return "PRETO"
    if (r, g, b) == (255, 255, 255): return "BRANCO"
    if (r, g, b) == (168, 230, 29): return "VERDE"
    if (r, g, b) == (237, 28, 36): return "VERMELHO"
    if (r, g, b) == (255, 242, 0): return "AMARELO"
    if (r, g, b) == (77, 109, 243): return "AZUL"
    # Se a cor não estiver na lista, retorna CRGB direto
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

