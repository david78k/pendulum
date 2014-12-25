(define innovs (new_innov_list))
(define g  (new_Genome_fromfile "testout" 1))
(define n (Genome_genesis g 1))
(Genome_mutate_add_link g innovs (testdoub) 3)

(print_Genome g)
(print_Genome_tofile g "gtestout")

(define pop1 (new_Pop_fromGenomefile "xorstartgenes"))
(xor_epoch pop1 1 "guilegen_1")

(define g  (new_Genome_fromfile "champ" 78))
(define n (Genome_genesis g 1))
(Network_graph n 500 500)

(define g  (new_Genome_load "champ"))
(define n (Genome_genesis g 1))
(Network_graph n 500 500)


(define (netload n inputs)
  (Network_input_start n)
  (netload_help n inputs)
  (Network_activate n)
  (Network_show_activation n)
)

(define (netload_help n inputs)
  (if (null? inputs)
      #t
      (begin
	(Network_load_in n (car inputs))
	(netload_help n (cdr inputs)))))



guile> (netload n '(1 0 0))
Network id 0 outputs: ([Output #1: (N4, step 2:6.21493e-07)] )
guile> (netload n '(1 0 1))
Network id 0 outputs: ([Output #1: (N4, step 3:1)] )
1
guile> (netload n '(1 1 0))
Network id 0 outputs: ([Output #1: (N4, step 4:0.999439)] )
1
guile> (netload n '(1 1 1))
Network id 0 outputs: ([Output #1: (N4, step 5:0.0080928)] )
guile> (define innovs (new_innov_list))
guile> (Genome_mutate_add_node g innovs (testdoub) (testdoub))
guile> (Genome_mutate_add_node g innovs (testdoub) (testdoub))
0
guile> (Genome_mutate_add_node g innovs (testdoub) (testdoub))
0
guile> (Genome_mutate_add_node g innovs (testdoub) (testdoub))
1
guile> (netload n '(1 0 0))
Network id 0 outputs: ([Output #1: (N4, step 1:7.47837e-12)] )
1
guile> (netload n '(1 0 1))
Network id 0 outputs: ([Output #1: (N4, step 2:1)] )
1
guile> (netload n '(1 1 0))
Network id 0 outputs: ([Output #1: (N4, step 3:0.0209742)] )
1
guile> (netload n '(1 1 1))
Network id 0 outputs: ([Output #1: (N4, step 4:0.00815414)] )
1

;A test showing old net and new nets activated on 0.5
guile> (netload n2 '(0.5 0.5 0.5))
Network id 134990848 outputs: ([Output #1: (N4, step 5:1.06854e-05)] )
1
guile> (define g3 (new_Genome_fromfile "goodxor_genome" 76)) 
guile> (define n3 (Genome_genesis g3 3))
guile> (netload n3 '(0.5 0.5 0.5))     ;The original
Network id 691216435 outputs: ([Output #1: (N4, step 1:1.09496e-05)] )
1
guile> (netload n '(0.5 0.5 0.5))
Network id 0 outputs: ([Output #1: (N4, step 9:2.56098e-08)] )


(define g2  (new_Genome_fromfile "xor_optimal" 33))
(define n2 (Genome_genesis g2 1))
(Network_graph n2 500 500)

(netload n2 '(1 0 0))
(Network_activate n2)
(netload n2 '(1 0 1))
(Network_activate n2)
(netload n2 '(1 1 0))
(Network_activate n2)
(netload n2 '(1 1 1))
(Network_activate n2)










