
conteudo = ""
arquivo = "include_tileloadds.h"

clsize = 64
matsize = 1024
with open(arquivo, "w") as file:
    for j in range(50):
        for i in range(0, matsize*50, clsize):   #64 tamanho do Cache Line
            conteudo+=(f"\"mov ${hex(i)}, %%rdx\\n\"\n")
            conteudo+=("\"tileloadd (%0, %%rdx, 1),%%tmm0\\n\"\n")
            
        for i in range(matsize*50, 0, -clsize):
            conteudo+=(f"\"mov ${hex(i)}, %%rdx\\n\"\n")
            conteudo+=("\"tileloadd (%0, %%rdx, 1),%%tmm0\\n\"\n")

    file.write(conteudo + "\n")

