

% cd  ('D:\Desenvolupament\Bronquis\SegmentacioCamiMesCurt\DebCTs\Dades\CAS26');
% load('CTBronchiEners_15_05_2017.mat');
% load('DistalComplexityLeaf_6_11_2017.mat');
% load('DistalGlobalSegLeaf_2_11_2017.mat')
% cd  ('D:\Desenvolupament\Bronquis\SegmentacioCamiMesCurt\DebCTs\Code');

%--------------
% error en el CASE 23 branca 3 al calcular el graph loops
% fem un remove i al refer el graf surten inf i peta

%--------------
% en el CASE34 hi ha una branca on el root és el que té minima energia i
% ens ho carreguem tot
%-------------
    

show_graphs = false;
for c=21:40

        
         %c = 34;
        str_case = ['CASE',int2str(c)];
        disp(str_case);
        path_data = ['D:\Desenvolupament\Bronquis\SegmentacioCamiMesCurt\DebCTs\Dades_noves\',str_case];
        load([path_data, filesep, 'CTBronchiEners_15_05_2017.mat']);
        load([path_data, filesep,'DistalComplexityLeaf_6_11_2017.mat']);
        load([path_data, filesep,str_case,'MainLocalSegLeaf_16_10_2017.mat'])



        %% COMPUTE ENERGY, STORE INSIDE GRAPH 

        BronchiEnerNorm = BronchiEner/max(BronchiEner(:));
        %BronchiEnerNorm =MainAirwaysEner/max(MainAirwaysEner(:));

        for k=1:numel(GSub)
            TG=GSub{k};
            if(prod(size(TG.e))>1)
                nnodes = size(TG.v,1);
                v=[];
                for i=1:nnodes
                    p = round([TG.v(i,1),TG.v(i,2),TG.v(i,3)]);
                    v(i) = max(BronchiEnerNorm(p(1),p(2),p(3)),MainLocalTh(p(1),p(2),p(3)));   
                   % v(i) = BronchiEnerNorm(p(1),p(2),p(3));         
                end
                GSub{k}.wv=[GSub{k}.wv,v(:)];
            end
        end




        %% LOOP NODES

        %LoopNodes_all: guardem per cada branca els nodes que formen loops
        LoopNodes_all=[];
        for k=1:numel(GSub)
        TG=GSub{k};
        LoopNodes_all{k}=[];
            if(prod(size(TG.e))>1)
                TG.e = Graph2dirGraph(TG.e,TG.rootNode);
                [ GSubLeak ] = GraphLoops( TG ); 
                LoopNodes = [];
                for kL=1:length(GSubLeak)
                    % Common Path: GSubLeak{kL}.LoopNodes contains nodes for GSubLeak{kL}.LeafNode
                    LoopNodes=[LoopNodes GSubLeak{kL}.LoopNodes];
                end
                LoopNodes_all{k} = LoopNodes;
            end
        end




        %% SHOW GRAF BRANCHES
        % 
        % %mostra el graf amb gris segons l'energia 
        % %del node vora vermella si és loop
        % 
        % for k=1:numel(GSub)
        %     f_show_graph(GSub{k}, LoopNodes_all{k});
        % end




        %% REMOVE  BRANCH LOOPS

        branques_on_hi_ha_loops = [];
        for i=1:length(LoopNodes_all)  
            if isempty(LoopNodes_all{i})==false 
                 branques_on_hi_ha_loops = [branques_on_hi_ha_loops, i]; 
            end 
        end

        %desem els nodes que eliminarem i els grafs
        nodes_elim_all =[];
        TG_elim_all =[];
        for k=1:numel(GSub)
              nodes_elim_all{k}=[];
              TG_elim_all{k} =[];
        end

        %per cada branca eliminem els loops
        for k=1:length(branques_on_hi_ha_loops)  
            i = branques_on_hi_ha_loops(k);
            TG = GSub{i};
            [nodes_elim, TG] = f_elimina_loops_branca(TG);
            nodes_elim_all{i}= nodes_elim;
            TG_elim_all{i} = TG;
        end



        %% SHOW RESULTS

        if (show_graphs==true)
             %veure el grafs de les branques abans i desp´res
            for k=1:length(branques_on_hi_ha_loops)  
                  i = branques_on_hi_ha_loops(k);
                %disp('branca');
                %disp(i);

                 f_show_graph(GSub{i}, LoopNodes_all{i});
                 f_show_graph(TG_elim_all{i}, LoopNodes_all{i});

            %      view(biograph(Graph2dirGraph(GSub{i}.e,GSub{i}.rootNode))); 
            %      view(biograph(Graph2dirGraph(TG_elim_all{i}.e,TG_elim_all{i}.rootNode))); 

            end
        end


        save([path_data, filesep,'leaks'], 'nodes_elim_all','TG_elim_all','LoopNodes_all','branques_on_hi_ha_loops', 'GSub');

        clearvars -except c show_graphs;
        close all;


end


