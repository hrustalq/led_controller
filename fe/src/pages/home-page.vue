<template>
  <div class="hosts__header">
    <h2 class="hosts__title">Список хостов:</h2>
  </div>
  <div class="hosts__list">
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
.hosts__title {
  text-align: center;
  margin: 2em 0;
}
.hosts__list {
  display: flex;
  flex-wrap: wrap;
  justify-content: center;
  width: 100%;
  gap: 1.5em;
}
</style>
