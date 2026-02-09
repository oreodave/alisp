;;; Directory Local Variables            -*- no-byte-compile: t -*-
;;; For more information see (info "(emacs) Directory Variables")

((nil . ((compile-command . "make MODE=debug test examples")
         (+license/license-choice . "GNU General Public License Version 2")))
 (c-mode . ((mode . clang-format)))
 ("test" .
  ((nil . ((compile-command . "make MODE=full test"))))))
