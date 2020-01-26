set number
syntax on
set tabstop=2
set softtabstop=2
set shiftwidth=2
set noexpandtab

set ruler
set colorcolumn=110

" Don't write code past this line bro!
highlight ColorColumn ctermbg=darkgray

" I'm writing c not c++!
augroup project
  autocmd!
  autocmd BufRead,BufNewFile *.h,*.c set filetype=c.doxygen
augroup END

set autochdir
set tags=../tags
