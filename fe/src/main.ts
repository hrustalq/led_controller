import { createApp } from 'vue'
import './style.css'
import App from './App.vue'

import { router } from './providers';

import { createPinia } from 'pinia';

import { createVuetify } from 'vuetify';
import * as components from 'vuetify/components'
import * as directives from 'vuetify/directives'
import { aliases, mdi } from 'vuetify/iconsets/mdi'

const pinia = createPinia();

const vuetify = createVuetify({
  components,
  directives,
  icons: {
    defaultSet: 'mdi',
    aliases,
    sets: {
      mdi,
    }
  },
})

const app = createApp(App);

app
  .use(vuetify)
  .use(pinia)
  .use(router)
  .mount('#app')
