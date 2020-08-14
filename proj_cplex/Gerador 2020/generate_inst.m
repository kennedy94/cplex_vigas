%Entrada: 
%   C = Número de tipos de vigas
%   M = Número de formas
%   arq = Nome do arquivo a ser gerado. Ex: 'inst1.txt'
%   misturado = 1 se tempo de cura são misturados, 0 para usar o padrão
%       Por padrão o tipo de viga i \in C terá tempo de cura i.
%   limite_cura = tempo de cura maximo quando misturado for usado.
%   up_d = limite superior de demandas
function gerar_inst(C, M, arq, misturado, down_d, up_d)
  pkg load statistics
    clc;
    %M = 7;
	if misturado == 1
        %limite_cura = input('Digite um limite de tempo de cura:');
        limite_cura = 3;
	end
    
    %Dados usados para criar conjunto de formas
    caps = [60];
    %c = datasample(caps, M);
    c = ones(M,1)*caps;
    
 
	t = 0;
 
	for i = 1:C
        %usar tempo de cura randomico de no maximo limite_cura se for
        %misturado
        if misturado == 1
            cura = ceil(rand(1,1)*limite_cura);
        else
            cura = i;
        end
        
        %dados usado para criar conjunto de tamanhos
        tams = [1.15 2.5 2.9 3.05 3.1 3.2 3.65 3.8 3.95 4.05 4.35 4.6 5.05 5.6 5.7 5.95 6 6.45 6.65 6.9 7.15];
        %tam = unique(datasample(tams, length(tams)));
        tam = tams;
        pegue_um_n = [4 5];
        peguei_um_n = randsample(pegue_um_n,1);
        tam = sort(randsample(tams,peguei_um_n,replacement=false));
        
        
        N = length(tam);

        d = randi([down_d up_d], 1, N);
        
        %estrutura usada para guardar dados do tipo gerado para a impressão
        s(i) = struct('cura', {cura},'n_tam', {N}, 'tamanhos', {tam}, 'demandas', {d});
        %calculo do T
        t = cura * sum(tam.*d) + t;
	end
 
	t = ceil(t/ sum(c));
    %aumento de 10% no T
    if t < 10
      t = t + 1;
    else
      t = ceil(0.10 * t) + t;
    end

 
    %impressão dos dados
	inst2 = fopen(arq, 'w');
	fprintf(inst2, '%d \n%d \n%d\n \n', C, M, t);
	for i = 1:C
        fprintf(inst2, '%d \n%d \n', s(i).cura, s(i).n_tam);
        for j = 1:s(i).n_tam
            fprintf(inst2, '%5.2f ', s(i).tamanhos(j)); 
        end
        fprintf(inst2, '\n');
        for j = 1:s(i).n_tam
            fprintf(inst2, '%d ', s(i).demandas(j));  
        end
        fprintf(inst2, '\n\n');
	end
 
	for i = 1:M
        fprintf(inst2, '%5.2f ', c(i));  
	end
	fclose(inst2);
end