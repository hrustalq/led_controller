<template>
  <div class="controllers__header">
    <h2 class="controllers__title">Список контроллеров:</h2>
  </div>
  <div class="controllers__list">
    <HostCard v-for="host in hosts" :host="host" :key="host.id" />
  </div>
</template>

<script setup lang="ts">
import HostCard from "../components/host-card.vue";
import { useHostsStore } from "../stores/hosts.store";
import { storeToRefs } from "pinia";

const { hosts } = storeToRefs(useHostsStore());
</script>

<style scoped>
.controllers__title {
  text-align: center;
  margin: 2em 0;
}
.controllers__list {
  display: flex;
  flex-wrap: wrap;
  justify-content: center;
  width: 100%;
  gap: 1.5em;
}
</style>
