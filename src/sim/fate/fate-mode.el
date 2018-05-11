(defun fate-mode ()
  "Major mode for editing Fate code."
  (interactive)
  (kill-all-local-variables)
  (c++-mode)
  (setq major-mode 'fate-mode
        mode-name "Fate")
  (setq comment-start "// "
        comment-end ""
        comment-multi-line nil
        c-double-slash-is-comments-p t)
)

(let ((comments     '(("/\\*" "\\*/" comment)))
      (c++-comments '(("//.*$" nil comment)
                      ("^/.*$" nil comment)))
      (strings      '((hilit-string-find ?' string)))
      (preprocessor '(("^#[ \t]*\\(undef\\|define\\|include\\).*$" "[^\\]$" define))))

  (hilit-set-mode-patterns
   'fate-mode
   (append
    comments c++-comments strings preprocessor
    '(
      ;; function decls are expected to have types on the previous line
      ("^\\(\\(\\w\\|[$_]\\)+::\\)?\\(\\w\\|[$_]\\)+\\s *\\(\\(\\w\\|[$_]\\)+\\s *((\\|(\\)[^)]*)+" nil defun)
      ("^\\(\\(\\w\\|[$_]\\)+[ \t]*::[ \t]*\\)?\\(\\(\\w\\|[$_]\\)+\\|operator.*\\)\\s *\\(\\(\\w\\|[$_]\\)+\\s *((\\|(\\)[^)]*)+" nil defun)
      ("^\\(template\\|typedef\\|struct\\|union\\|class\\|enum\\|public\\|private\\|protected\\).*$" nil decl)
      ;; datatype -- black magic regular expression
      ("[ \n\t({]\\(\\(const\\|register\\|volatile\\|unsigned\\|extern\\|static\\)\\s +\\)*\\(\\(\\w\\|[$_]\\)+_t\\|float\\|double\\|void\\|char\\|short\\|int\\|long\\|FILE\\|\\(\\(struct\\|union\\|enum\\|class\\)\\([ \t]+\\(\\w\\|[$_]\\)*\\)\\)\\)\\(\\s +\\*+)?\\|[ \n\t;()]\\)" nil type)
      ;; key words
      ("[^_]\\<\\(return\\|goto\\|if\\|else\\|case\\|default\\|switch\\|break\\|continue\\|while\\|do\\|for\\|public\\|protected\\|private\\|forall\\|group\\|link\\|snap\\|snapshot\\|in\\|exec\\|begin\\|end\\|birth\\|death\\|proc\\|choose\\|program\\|timeseq\\|explicit\\|periodic\\|uniform\\|poisson\\|clear\\|selector\\|inject\\|call\\|diag\\|with\\|<-\\|fstream\\|delete\\|new\\)\\>[^_]"  1 keyword)))
   )
)

(setq auto-mode-alist (cons '("\\.fc$" . fate-mode) auto-mode-alist))
