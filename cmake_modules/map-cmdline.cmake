changeset = 'changeset:   {rev}:{node|short}\n{branches}{bookmarks}{tags}{parents}user:        {author}\ndate:        {date|shortdate}\nsummary:     {desc|firstline}\n\n'
changeset_quiet = '{rev}:{node|short}\n'
changeset_verbose = 'changeset:   {rev}:{node|short}\n{branches}{bookmarks}{tags}{parents}user:        {author}\ndate:        {date|date}\n{files}{file_copies_switch}description:\n{desc|strip}\n\n\n'
changeset_debug = 'changeset:   {rev}:{node}\n{branches}{bookmarks}{tags}{parents}{manifest}user:        {author}\ndate:        {date|date}\n{file_mods}{file_adds}{file_dels}{file_copies_switch}{extras}description:\n{desc|strip}\n\n\n'
start_files = 'files:      '
file = ' {file}'
end_files = '\n'
start_file_mods = 'files:      '
file_mod = ' {file_mod}'
end_file_mods = '\n'
start_file_adds = 'files+:     '
file_add = ' {file_add}'
end_file_adds = '\n'
start_file_dels = 'files-:     '
file_del = ' {file_del}'
end_file_dels = '\n'
start_file_copies = 'copies:     '
file_copy = ' {name} ({source})'
end_file_copies = '\n'
parent = 'parent:      {rev}:{node|formatnode}\n'
manifest = 'manifest:    {rev}:{node}\n'
branch = 'branch:      {branch}\n'
tag = 'tag:         {tag}\n'
bookmark = 'bookmark:    {bookmark}\n'
extra = 'extra:       {key}={value|stringescape}\n'
