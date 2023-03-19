(setq *toggle-quote* 0)

(defun gppinterpreter(&optional filename)
	(let ((out (open "output.txt" :direction :output)))
		(if filename (file-lexer filename out) (terminal-lexer out))
		(close out)
	)
)

(defun file-lexer(filename out)
	(let ((in (open fileName :if-does-not-exist nil)))
		(when in
			(loop for line = (read-line in nil)
				while line do
				(unless (tokenize line out) (return ))
			)
			(close in)
		)	(unless in
			(format t "No such file ~a found..." filename)
		)
	)
)

(defun terminal-lexer(out)
	(loop for line = (read-line ) 
		while (not (string= line "")) 
		do 
		(unless (tokenize line out) (return ))
	)
)

(defun tokenize(line out)
	(let ((tokens (str-to-list line)) (speckey))
		(loop for token in tokens
			do
			(unless (is-empty-str token)					;; if token != ""
				(setq speckey (tokenize-special-keys token))
				(if speckey (write-line speckey out)
					(progn
						(cond
							((tokenize-comment token)
							(write-line "COMMENT" out)
							)
							((tokenize-value token)
								(write-line "VALUE" out)
							)
							((tokenize-identifier token)
								(write-line "IDENTIFIER" out)
							)
							(t
								(format t "SYNTAX ERROR ~a cannot be tokenized" token)
								(return-from tokenize nil)
							)		
						)
					)
				)
			)
		)
		(return-from tokenize t)
	)
)

(defun str-to-list(str)
	(unless (is-empty-str str)
		(let ((result (detect-pos str)) (chr) (pos))

			(unless (null result) (setq chr (car result)) (setf pos (cdr result)))			;; [0]->char, [1]->pos

			(cond
				((null result) (list str))									;; if no special char is found in the line
				;; chr == "(" or ")"
				((is-bracket chr)
					(if (= pos 0)
						(cons (subseq str 0 1) (str-to-list (subseq str 1)))
						(cons (subseq str 0 pos) (str-to-list (subseq str pos)))
					)
				)
				;; chr == " "
				((is-space chr)
					(if (= pos 0)
						(str-to-list (subseq str 1))
						(cons (subseq str 0 pos) (str-to-list (subseq str pos)))
					)
				)
				;; chr == "\""
				((is-quote chr)
					(if (= pos 0)
						(cons (subseq str 0 1) (str-to-list (subseq str 1)))
						(cons (subseq str 0 pos) (str-to-list (subseq str pos)))
					)
				)
				;; chr == ";"
				((is-semicolon chr)
					(if (and (not (is-empty-str (subseq str pos))) (is-semicolon (char str (+ 1 pos))))
						(list (subseq str 0 pos) (subseq str pos))
					)
				)
			)
		)
	)
)



(defun detect-pos(str)
	(when (not (is-empty-str str))
		(loop for x from 0 to (- (length str) 1)
			do
			(if (or (is-space (char str x)) (is-bracket (char str x)) (is-quote (char str x)) (is-semicolon (char str x)))
				(return-from detect-pos (cons (char str x) x))
			)
		)
	)
)

(defun tokenize-comment(token)
	(and (> (length token) 1) (is-semicolon (char token 0)) (is-semicolon (char token 1)))
)

(defun tokenize-identifier(token)
	(unless (is-alpha (char token 0)) (return-from tokenize-identifier))
	(loop for i across token
		do
		(if (not (is-alpha-numeric i)) (return-from tokenize-identifier))
	)
	(return-from tokenize-identifier t)
)

(defun tokenize-value(token)
	(when (and (char= (char token 0) #\0) (> (length token) 1)) (return-from tokenize-value))
	(loop for i across token
		do
		(if (not (is-digit i))
			(return-from tokenize-value)
		)
	)
	(return-from tokenize-value t)
)

(defun tokenize-special-keys(token)
	(let ((keys (get-special-keys)) (return-val) )
		(if (string= token "\"") 
			(progn
				(if (= *toggle-quote* 0)
					(progn
						(setq *toggle-quote* 1)
						(return-from tokenize-special-keys "OP_OC")
					)
					(progn
						(setq *toggle-quote* 0)
						(return-from tokenize-special-keys "OP_CC")
					)
				)
			)
		)
		(setq return-val (assoc token keys :test #'string=))
		(when return-val (return-from tokenize-special-keys (cdr return-val)))
	)
)

(defun get-special-keys()
	(pairlis '("and" "or" "not" "equal" "less" "nil" "list" "append" "concat" "set" "deffun" "for" "if" "exit" "load" "disp" "true" "false" "+" "-" "/" "*" "(" ")" "**" "," "\"" "\"") 
	'("KW_AND" "KW_OR" "KW_NOT" "KW_EQUAL" "KW_LESS" "KW_NIL" "KW_LIST" "KW_APPEND" "KW_CONCAT" "KW_SET" "KW_DEFFUN" "KW_FOR" "KW_IF" "KW_EXIT" "KW_LOAD" "KW_DISP" "KW_TRUE" "KW_FALSE" "OP_PLUS" "OP_MINUS" "OP_DIV" "OP_MULT" "OP_OP" "OP_CP" "OP_DBLMULT" "OP_COMMA" "OP_OC" "OP_CC")) 
)

(defun is-alpha-numeric(chr)
	(or (is-alpha chr) (is-digit chr))
)

(defun is-alpha(chr)
	(or (and (char>= chr #\a) (char<= chr #\z)) (and (char>= chr #\A) (char<= chr #\Z)) (char= chr #\_))
)

(defun is-digit(chr)
	(and (char>= chr #\0) (char<= chr #\9))
)

(defun is-space (chr)
	(or (and (char>= chr (code-char 9)) (char<= chr (code-char 13))) (char= chr (code-char 32)))
)

(defun is-bracket (chr)
	(or (char= chr #\() (char= chr #\)))
)

(defun is-quote (chr)
	(char= chr #\")
)

(defun is-semicolon (chr)
	(char= chr #\;)
)

(defun is-empty-str (str)
	(= 0 (length str))
)


(gppinterpreter (car *args*))
