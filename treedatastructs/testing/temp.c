/ creates a new node if the word is not found
   if (root == NULL){     
      Link new = newNode();
      new -> data.word = duplicate(w);
      new -> data.freq++;
      *freq = &(new -> data);
      return new;
   }
   // traverses the tree based on alphabetical order
   int alphaorder = strcmp(root -> data.word, w);

   // updates the frequency of the word is found
   if (alphaorder == 0){
      root -> data.freq++;
      *freq = &(root -> data);
      return root;
   }
   int dir = 0;
   // continues left if word is below alphabetically
   if (alphaorder > 0){
      root -> left = DictSearch(root -> left, w, freq);
      dir = strcmp(root -> left -> data.word, w);
   }
   // countinues right if word is in front alphabetically
   else if (alphaorder < 0){
      root -> right = DictSearch(root -> right, w, freq);
      dir = strcmp(root -> right -> data.word, w);
   }
   root = reBalance(root, dir); //rebalances and updates the root
   return root;

   