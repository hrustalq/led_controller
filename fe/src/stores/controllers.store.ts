import { ref } from "vue";

import { defineStore } from "pinia";

export const useControllersStore = defineStore("controllers", () => {
  const controllers = ref([]);

  return {
    controllers,
  };
});
