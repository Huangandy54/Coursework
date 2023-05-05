// This will be the object that will contain the Vue attributes
// and be used to initialize it.
let app = {};

// Given an empty app object, initializes it filling its attributes,
// creates a Vue instance, and then initializes the Vue instance.
let init = (app) => {

    // This is the Vue data.
    app.data = {
        posts: [],
    };

    app.index = (a) => {
        // Adds to the posts all the fields on which the UI relies.
        let i = 0;
        for (let p of a) {
            p._idx = i++;
            p.editable = (p.email==user_email) ? true : false;
            p.edit = false;
            p.tedit = false;
            p.is_pending = false;
            p.error = false;
            p.fav = p.favorite;
            p.original_content = p.content; // Content before an edit.
            p.server_content = p.content;
            p.original_title = p.title; // title before an edit.
            p.server_title = p.title; // title on the server. 
            p.current_color = p.color;
        }
        return a;
    };


    app.reindex = () => {
        // Adds to the posts all the fields on which the UI relies.
        let i = 0;
        for (let p of app.vue.posts) {
            p._idx = i++;
        }
    };
    // Handler for button that starts the edit.
    app.do_edit = (post_idx) => {
        
        // no OTHER post is being edited.
        
        let p = app.vue.posts[post_idx];
        //check to not allow other ppl to edit posts that arent theirs
        if (p.email==user_email){
            //check + boolean for determining if something else is in edit mode
            let somethinginedit=false;
            for (let p of app.vue.posts) {
                if(p.edit==true){
                    somethinginedit=true;
                }
            }

            if (somethinginedit==false) {
                let p = app.vue.posts[post_idx];
                p.edit = true;
                p.is_pending = false;
            }
        }
        
        
    };

    app.do_edit_title = (post_idx) =>{
        let p = app.vue.posts[post_idx];
        //check to not allow other ppl to edit posts that arent theirs
        if (p.email==user_email){
            //check + boolean for determining if something else is in edit mode
            let somethinginedit=false;
            for (let p of app.vue.posts) {
                if(p.tedit==true){
                    somethinginedit=true;
                }
            }

            if (somethinginedit==false) {
                let p = app.vue.posts[post_idx];
                p.tedit = true;
                p.is_pending = false;
            }
        }
    };
    // cancels the edit.
    app.do_cancel = (post_idx) => {
        
        let p = app.vue.posts[post_idx];
        if (p.id === null) {
            // If the post has not been saved yet, we delete it.
            app.vue.posts.splice(post_idx, 1);
            app.reindex();
        } else {
            // We go back to before the edit.
            p.edit = false;
            p.tedit=false;
            p.is_pending = false;
            p.content = p.original_content;
            p.title=p.original_title;
        }
    }

    // "Save edit" button.
    app.do_save = (post_idx) => {
        
        let p = app.vue.posts[post_idx];
        if (p.content !== p.server_content || p.title !== p.server_title) {
            p.is_pending = true;
            axios.post(posts_url, {
                content: p.content,
                title: p.title,
                id: p.id,
            }).then((result) => {
                console.log("Received:", result.data);
                // receiving the post id (in case it was inserted),
                // and the content. set both, and to say that
                // the editing has terminated.
                p.id=result.data.id;
                p.server_content=result.data.content;
                p.server_title=result.data.title;
                p.is_pending=false;
                p.edit=false;
                p.tedit=false;
            }).catch(() => {
                p.is_pending = false;
                console.log("Caught error");
                // We stay in edit mode.
            });
        } else {
            // No need to save.
            p.edit = false;
            p.tedit =false;
            p.original_content = p.content;
            p.original_title = p.title;
        }
    }


    app.add_post = () => {
        let q = {
            id: null,
            edit: true,
            tedit : true,
            editable: true,
            title: "",
            content: "",
            fav: false,
            current_color : "is-dark",
            server_content: null,
            original_content: "",
            author: author_name,
            email: user_email,
        };
        // insert at the top of the post list.
        app.vue.posts.splice(0,0,q);
        app.reindex();
    };


    app.do_delete = (post_idx) => {
        let confirmation=confirm("Are you sure you want to delete this note?");
        if (confirmation) {
            let p = app.vue.posts[post_idx];
            if (p.id === null) {
                // If the post has never been added to the server,
                // simply deletes it from the list of posts.
                // deletes at post_idx by overwritting with nothing
                app.vue.posts.splice(post_idx,1);
            } else {
                // Deletes on the server.
                tempData={
                    id : post_idx
                }
                axios.post(delete_url,{
                    id: p.id
                }).then(() => {
                    app.vue.posts.splice(post_idx,1);
                })
            }
        }
        
    };

    app.change_color=(post_idx, newcolor)=>{
        let p = app.vue.posts[post_idx];
        p.current_color=newcolor;
        axios.post(color_url,{
            id : p.id,
            new_color : newcolor
        }).then(() => {
            console.log("server updated");
        })
    };


    app.change_fav_status=(post_idx)=>{
        let p = app.vue.posts[post_idx];
        opposite=!p.fav;
        axios.post(favorite_url,{
            id : p.id,
            new_val : opposite
        }).then(() => {
            p.fav=opposite
            console.log("server updated");
        })
    };
    // We form the dictionary of all methods, so we can assign them
    // to the Vue app in a single blow.
    app.methods = {
        do_edit: app.do_edit,
        do_cancel: app.do_cancel,
        do_save: app.do_save,
        add_post: app.add_post,
        do_delete: app.do_delete,
        change_color : app.change_color,
        do_edit_title : app.do_edit_title,
        change_fav_status : app.change_fav_status,
    };

    // This creates the Vue instance.
    app.vue = new Vue({
        el: "#vue-target",
        data: app.data,
        methods: app.methods
    });

    // And this initializes it.
    app.init = () => {
        axios.get(posts_url).then((result) => {
            app.vue.posts = app.index(result.data.posts);
        })
        
    };

    // Call to the initializer.
    app.init();
};

// This takes the (empty) app object, and initializes it,
// putting all the code i
init(app);
